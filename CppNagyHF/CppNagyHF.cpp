
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
#include "Manager.hpp"

int main(int argc, char* argv[])
{

    srand((uint32_t)std::chrono::system_clock::now().time_since_epoch().count());

    Manager::MainWithUI(argc, argv);

    return 0;

    //auto game = game::Game(true, game::GameControlType::KEYBOARD, 8, 8, 800, 800);

    //game.SetSpeed(5);

    //game.Start();

    //return 0;

    model::NeatTrainer trainer("../CppNagyHF/hyperparams.json");

    trainer.Train();
    
    while (true) {

        auto bestModel = trainer.GetModelFromBestSpeciesInLastGeneration();

        auto game = game::Game(true, game::GameControlType::AI, 7, 7, 1000, 1000, *bestModel, 100, false);

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