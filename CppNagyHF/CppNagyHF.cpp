
#include <iostream>
#ifndef CPORTA
#include <SFML/Graphics.hpp>
#endif
#include "vector.hpp"
#include "Game.hpp"
#include <chrono>
#include "RandomModel.hpp"
#include "NeatModel.hpp"
#include "NeatTrainer.hpp"
#include "SpeciesData.hpp"
#include <fstream>

int main()
{

    srand((uint32_t)std::chrono::system_clock::now().time_since_epoch().count());

    //auto game = game::Game(true, game::GameControlType::KEYBOARD, 3, 3, 800, 800);

    //game.SetSpeed(2);

    //game.Start();

    //return 0;
    
    const auto activationFunction = std::make_shared<model::Sigmoid>();
    const auto fitnessFunction = model::FitnessByApplesAndSteps<1000, 1>();

  
    std::unordered_map<long long, int> table;
    model::NeatModel model(activationFunction, table);

    std::ofstream outFile("test.model", std::ios::binary);

    model.Serialize(outFile);

    outFile.close();

    std::ifstream inFile("test.model", std::ios::binary);

    model::NeatModel loadedModel(inFile);

    inFile.close();

    std::cout << model << std::endl;
    std::cout << loadedModel << std::endl;

    model::ModelParams params;
    params.SetToRandom();

    for (const double res : model.Predict(params))
        std::cout << res << std::endl;
    
    std::cout << "-------" << std::endl;

    for (const double res : loadedModel.Predict(params))
        std::cout << res << std::endl;
    
    std::cout << "-------" << std::endl;
    
    for (const auto& gene : model.Genes())
        std::cout << gene << std::endl;

    std::cout << "-------" << std::endl;

    for (const auto& gene : loadedModel.Genes())
        std::cout << gene << std::endl;

    return 0;

    //const auto fitnessFunction = model::FitnessByApplesAndSteps<1000, 1>();

    auto trainer = model::NeatTrainer(
        200, 
        300,
        activationFunction, 
        40, 
        6, 
        6, 
        &fitnessFunction
    );

    trainer.chanceOfDentritInsertion = 0.09;
    trainer.chanceOfNeuronInsertion = 0.008;
    trainer.portionOfSpeciesToKeepForReproduction = 0.3;
    trainer.chanceOfDisabling = 0.02;
    trainer.chanceOfDentritMutation = 0.2;
    trainer.weightAdjustMin = -0.2;
    trainer.weightAdjustMax = 0.2;
    trainer.minImprovementOfAvgFitnessToConsiderItAnImprovement = 0.01;
    trainer.numGenerationsWithSameFitnessBeforeOnlyLookingAtTopSpecies = 20;
    trainer.numberOfTopSpeciesToLookAtIfFitnessIsStableForTooLong = 5;
    trainer.chanceOfGeneDisablingIfEitherGeneIsDisabled = 0.75;
    trainer.chanceOfMutationBeingNewValue = 0.1;
    trainer.weightSetMax = 1;
    trainer.weightSetMin = -1;
    trainer.placeFirstAppleInFrontOfSnake = false;
    
    trainer.speciesDropOffAge = 15;
    trainer.speciesDropOffFitnessThreshold = 0.4;

    trainer.numberOfEvaluationSteps = 10;

    trainer.targetFitness = 18;

    trainer.SetNeatConstants(1, 1, 1.4, 0.4);

    trainer.Train();

#if XOR == 1
    return 0;
#endif

    while (true) {

        using model::SpeciesData;
        using model::NeatModel;

        SpeciesData* dataOfBestSpecies = std::max_element(
            trainer.speciesData.begin(),
            trainer.speciesData.end(),
            [&](const SpeciesData a, const SpeciesData b) {
                return a.lastFitness < b.lastFitness;
            });

        int specIndex = 0;

        for (int i = 0; i < trainer.speciesData.size(); i++)
            if (&trainer.speciesData[i] == dataOfBestSpecies)
                specIndex = i;

        NeatModel* bestModel = const_cast<NeatModel*>(trainer.representativesOfThePrevGeneration[specIndex]);

        std::cout << "Fitness: " << bestModel->rawFitness << std::endl;

        for (int i = 0; i < 5; i++)
            std::cout << "Score: " << trainer.EvaluateIndividual(*bestModel) << std::endl;

        auto game = game::Game(true, game::GameControlType::AI, 6, 6, 1000, 1000, *bestModel, 100, false);

        game.SetSpeed(8);

        game.Start();

        const auto report = game.GenerateReport();

        std::cout << report << std::endl;

        std::cout << fitnessFunction.operator()(report) << std::endl;

        std::cout << "Type in 'q' to quit... ";

        if (std::cin.get() == 'q')
            break;
    }

    return 0;
}