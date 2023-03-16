// CppNagyHF.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <SFML/Graphics.hpp>
#include "vector.hpp"
#include "Game.hpp"

int main()
{
    //cstd::Vector<int> v;

    //v += 13;
    //v += 14;

    //cstd::Vector<int> w;

    //w = v;

    //std::cout << v << std::endl;
    //std::cout << w << std::endl;

   
    auto game = game::Game(true, game::GameControlType::KEYBOARD, 10, 10, 800, 800);

    game.Start();

    return 0;
}