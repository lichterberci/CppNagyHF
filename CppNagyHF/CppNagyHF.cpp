
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

    model::NeatTrainer trainer("hyperparams.json");

    trainer.Train();

    //auto game = game::Game(true, game::GameControlType::KEYBOARD, 3, 3, 800, 800);

    //game.SetSpeed(2);

    //game.Start();

    //return 0;
    
    while (true) {

        auto bestModel = trainer.GetModelFromBestSpeciesInLastGeneration();

        auto game = game::Game(true, game::GameControlType::AI, 3, 3, 1000, 1000, *bestModel, 100, false);

        game.SetSpeed(8);

        game.Start();

        const auto report = game.GenerateReport();

        std::cout << report << std::endl;

        std::cout << "Type in 'q' to quit... ";

        if (std::cin.get() == 'q')
            break;
    }

    return 0;
}