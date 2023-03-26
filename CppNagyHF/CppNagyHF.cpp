
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

    srand(std::chrono::system_clock::now().time_since_epoch().count());

    //auto game = game::Game(true, game::GameControlType::AI, 10, 10, 800, 800);

    //game.SetSpeed(6);

    //game.Start();

    auto trainer = model::NeatTrainer(
        3, 
        2, 
        model::Sigmoid(), 
        100, 
        10, 
        10, 
        model::FitnessByApplesAndStepsAndWin<100, 1, 1000>()
    );


    return 0;
}