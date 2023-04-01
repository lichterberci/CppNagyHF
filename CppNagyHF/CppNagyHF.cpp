
#include <iostream>
#include <SFML/Graphics.hpp>
#include "vector.hpp"
#include "Game.hpp"
#include <chrono>
#include "RandomModel.hpp"
#include "NeatModel.hpp"
#include "NeatTrainer.hpp"

int main()
{

    srand((uint32_t)std::chrono::system_clock::now().time_since_epoch().count());

    //auto game = game::Game(true, game::GameControlType::AI, 10, 10, 800, 800);

    //game.SetSpeed(6);

    //game.Start();

    const auto activationFunction = model::Sigmoid();
    const auto fitnessFunction = model::FitnessByApplesAndSteps<100, 1>();

    auto trainer = model::NeatTrainer(
        400, 
        300, 
        &activationFunction, 
        100, 
        8, 
        8, 
        &fitnessFunction
    );

    trainer.chanceOfDentritInsertion = 0.1;
    trainer.chanceOfNeuronInsertion = 0.2;
    trainer.portionOfSpeciesToKeepForReproduction = 0.5;
    trainer.chanceOfMutation = 0.8;

    trainer.SetNeatConstants(1, 1, 0.4, 3);

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

    for (const auto& gen : trainer.organismsByGenerations) {

        /*for (const auto& organism : gen) {
            std::cout << std::setprecision(3) << organism.rawFitness << " ";
        }*/

        //std::cout << std::endl;

        const auto& best = std::max_element(gen.begin(), gen.end(), [](const model::NeatModel& a, const model::NeatModel& b) {
            return a.rawFitness < b.rawFitness;
        });

        std::cout << *best << std::endl;
    }

    while (true) {

        model::NeatModel* bestOfLastGen = std::max_element(trainer.organismsByGenerations.last().begin(), trainer.organismsByGenerations.last().end(), [](const model::NeatModel& a, const model::NeatModel& b) {
            return a.rawFitness < b.rawFitness;
        });

        auto game = game::Game(true, game::GameControlType::AI, 10, 10, 800, 800, *bestOfLastGen, 100);

        game.SetSpeed(5);

        game.Start();

        std::cout << "Type in 'q' to quit... ";

        if (std::cin.get() == 'q')
            break;
    }

    return 0;
}