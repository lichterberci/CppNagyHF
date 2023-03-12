// CppNagyHF.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <SFML/Graphics.hpp>
#include "vector.hpp"
#include "Game.hpp"

int main()
{
   
    auto game = game::Game(true, game::GameControlType::KEYBOARD);

    game.Start();

    return 0;
}