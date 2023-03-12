// CppNagyHF.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <SFML/Graphics.hpp>
#include "vector.hpp"
#include "Game.hpp"

int main()
{
   
    auto game = game::Game(true, 640, 480);

    game.Start();

    return 0;
}