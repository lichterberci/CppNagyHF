
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

int main()
{

    srand((uint32_t)std::chrono::system_clock::now().time_since_epoch().count());

    //auto game = game::Game(true, game::GameControlType::KEYBOARD, 10, 10, 800, 800);

    //game.SetSpeed(6);

    //game.Start();

    //return 0;

    const auto activationFunction = model::SteepSigmoid<int, 1>();
    const auto fitnessFunction = model::FitnessByApplesOnly();
    //const auto fitnessFunction = model::FitnessByApplesAndSteps<1000, 1>();

    auto trainer = model::NeatTrainer(
        100, 
        300,
        &activationFunction, 
        50, 
        3, 
        3, 
        &fitnessFunction
    );

    trainer.chanceOfDentritInsertion = 0.3;
    trainer.chanceOfNeuronInsertion = 0.02;
    trainer.portionOfSpeciesToKeepForReproduction = 0.3;
    trainer.chanceOfDisabling = 0.07;
    trainer.chanceOfDentritMutation = 0.2;
    trainer.weightAdjustMin = -0.5;
    trainer.weightAdjustMax = 0.5;
    trainer.minImprovementOfAvgFitnessToConsiderItAnImprovement = 0.01;
    trainer.numGenerationsWithSameFitnessBeforeOnlyLookingAtTopSpecies = 20;
    trainer.numberOfTopSpeciesToLookAtIfFitnessIsStableForTooLong = 5;
    trainer.chanceOfGeneDisablingIfEitherGeneIsDisabled = 0.75;
    trainer.chanceOfMutationBeingNewValue = 0.1;
    trainer.weightSetMax = 2;
    trainer.weightSetMin = -2;
    trainer.placeFirstAppleInFrontOfSnake = true;
    
    trainer.speciesDropOffAge = 15;
    trainer.speciesDropOffFitnessThreshold = 0.005;

    trainer.numberOfEvaluationSteps = 10;

    trainer.targetFitness = 3;

    trainer.SetNeatConstants(1, 1, 1, 0.75);

    //trainer.TrainCurrentGeneration();

    //for (const auto& organism : trainer.organismsByGenerations[0]) {
    //    std::cout << organism << std::endl;
    //    for (const auto& gene : organism.Genes())
    //        if (gene.innovationNumber > 90)
    //            std::cout << gene << std::endl;
    //}
    // 
    //std::cout << "---------------------------------------------------" << std::endl;

    //for (const auto& organism : trainer.organismsByGenerations[1]) {
    //    std::cout << organism << std::endl;
    //    for (const auto& gene : organism.Genes())
    //        if (gene.innovationNumber > 90)
    //            std::cout << gene << std::endl;
    //}

    //trainer.TrainCurrentGeneration();

    //std::cout << "---------------------------------------------------" << std::endl;

    //for (const auto& organism : trainer.organismsByGenerations[2]) {
    //    std::cout << organism << std::endl;
    //    for (const auto& gene : organism.Genes())
    //        if (gene.innovationNumber > 90)
    //            std::cout << gene << std::endl;
    //}

    trainer.Train();

#if XOR == 1
    return 0;
#endif

    for (const auto& gen : trainer.organismsByGenerations) {

        /*for (const auto& organism : gen) {
            std::cout << std::setprecision(3) << organism.rawFitness << " ";
        }*/

        //std::cout << std::endl;

      /*  const auto& best = std::max_element(gen.begin(), gen.end(), [](const model::NeatModel& a, const model::NeatModel& b) {
            return a.rawFitness < b.rawFitness;
        });

        std::cout << *best << std::endl;*/
    }

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

        auto game = game::Game(true, game::GameControlType::AI, 3, 3, 800, 800, *bestModel, 100, false);

        game.SetSpeed(5);

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