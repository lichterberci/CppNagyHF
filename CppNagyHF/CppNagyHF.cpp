// CppNagyHF.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <SFML/Graphics.hpp>
#include "vector.hpp"
#include "Game.hpp"
#include <chrono>
#include "RandomModel.hpp"
#include "NeatModel.hpp"

int main()
{

    srand(std::chrono::system_clock::now().time_since_epoch().count());

    auto neat = model::NeatModel(NUM_SENSORS, NUM_OUTPUTS, model::LReLU());

    auto game = game::Game(true, game::GameControlType::AI, 10, 10, 800, 800);

    game.AttachModel(neat);

    game.SetSpeed(10);

    game.Start();

    std::cout << game.GenerateReport() << std::endl;

    return 0;
}