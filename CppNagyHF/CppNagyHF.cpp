// CppNagyHF.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <SFML/Graphics.hpp>
#include "vector.hpp"
#include "Game.hpp"
#include <chrono>
#include "RandomModel.hpp"

int main()
{

    srand(std::chrono::system_clock::now().time_since_epoch().count());

    auto randomModel = model::RandomModel();
    auto game = game::Game(true, game::GameControlType::AI, 8, 8, 800, 800);

    game.SetSpeed(6);

    game.Start();

    return 0;
}