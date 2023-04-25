
#include <iostream>
#ifndef JPORTA
#include <SFML/Graphics.hpp>
#endif
#include "vector.hpp"
#include "Game.hpp"
#include <chrono>
#include "RandomModel.hpp"
#include "NeatModel.hpp"
#include "NeatTrainer.hpp"

int main()
{

    srand((uint32_t)std::chrono::system_clock::now().time_since_epoch().count());

    //auto game = game::Game(true, game::GameControlType::KEYBOARD, 10, 10, 800, 800);

    //game.SetSpeed(6);

    //game.Start();

    //return 0;

    const auto activationFunction = model::SteepSigmoid<int, 4>();
    const auto fitnessFunction = model::FitnessByApplesAndSteps<1000, 1>();

    auto trainer = model::NeatTrainer(
        50, 
        1000,
        &activationFunction, 
        40, 
        10, 
        10, 
        &fitnessFunction
    );

    trainer.chanceOfDentritInsertion = 0.00;
    trainer.chanceOfNeuronInsertion = 0.00;
    trainer.portionOfSpeciesToKeepForReproduction = 0.3;
    trainer.chanceOfDisabling = 0.0;
    trainer.chanceOfDentritMutation = 0.8;
    trainer.weightAdjustMin = -0.5;
    trainer.weightAdjustMax = 0.5;
    trainer.minImprovementOfAvgFitnessToConsiderItAnImprovement = 0.02;
    trainer.numGenerationsWithSameFitnessBeforeOnlyLookingAtTopSpecies = 500;
    trainer.chanceOfGeneDisablingIfEitherGeneIsDisabled = 0.75;
    trainer.chanceOfMutationBeingNewValue = 0.1;
    trainer.weightSetMax = 1;
    trainer.weightSetMin = -1;
    trainer.placeFirstAppleInFrontOfSnake = false;
    
    trainer.targetFitness = 0.95;

    trainer.SetNeatConstants(1, 1, 4, 1);

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

        model::NeatModel* bestOfLastGen = std::max_element(trainer.organismsByGenerations.last().begin(), trainer.organismsByGenerations.last().end(), [](const model::NeatModel& a, const model::NeatModel& b) {
            return a.rawFitness < b.rawFitness;
        });

        auto game = game::Game(true, game::GameControlType::AI, 10, 10, 800, 800, *bestOfLastGen, 100);

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