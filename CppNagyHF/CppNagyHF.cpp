
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
#include "HyperparameterLoader.hpp"

int main()
{

    srand((uint32_t)std::chrono::system_clock::now().time_since_epoch().count());

    model::HyperparameterLoader()("test.json");

    return 0;
    
    //auto game = game::Game(true, game::GameControlType::KEYBOARD, 3, 3, 800, 800);

    //game.SetSpeed(2);

    //game.Start();

    //return 0;
    
    const auto activationFunction = std::make_shared<model::Sigmoid>();
    const auto fitnessFunction = std::make_shared<model::FitnessByApplesAndSteps>(1000, 1);

    auto trainer = model::NeatTrainer(
        200, 
        100,
        activationFunction, 
        40, 
        6, 
        6, 
        fitnessFunction
        ,"test.progress"
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

    trainer.SaveProgress("test.progress");

    while (true) {

        auto bestModel = trainer.GetModelFromBestSpeciesInLastGeneration();

        auto game = game::Game(true, game::GameControlType::AI, 6, 6, 1000, 1000, *bestModel, 100, false);

        game.SetSpeed(8);

        game.Start();

        const auto report = game.GenerateReport();

        std::cout << report << std::endl;

        std::cout << fitnessFunction->operator()(report) << std::endl;

        std::cout << "Type in 'q' to quit... ";

        if (std::cin.get() == 'q')
            break;
    }

    return 0;
}