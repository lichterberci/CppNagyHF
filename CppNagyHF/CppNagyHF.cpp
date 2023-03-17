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
    auto game = game::Game(true, game::GameControlType::KEYBOARD, 10, 10, 800, 800);

    game.SetSpeed(7);

    game.Start();

    return 0;
}