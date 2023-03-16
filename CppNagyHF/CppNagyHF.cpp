// CppNagyHF.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <SFML/Graphics.hpp>
#include "vector.hpp"
#include "Game.hpp"
#include <chrono>

int main()
{
    //cstd::Vector<int> v;

    //v += 13;
    //v += 14;

    //cstd::Vector<int> w;

    //w = v;

    //std::cout << v << std::endl;
    //std::cout << w << std::endl;

    //sf::RenderWindow window(sf::VideoMode(800, 800), "Snake");
    //sf::View view = window.getDefaultView();

    //for (int i = 0; i < 800; i += 40)
    //    for (int j = 0; j < 800; j += 40) {
    //        sf::RectangleShape rect(sf::Vector2f(40, 40));
    //        rect.setPosition(sf::Vector2f(i, j));
    //        rect.setFillColor((i + j) / 40 % 2 == 0 ? sf::Color::Black : sf::Color::Green);
    //        window.draw(rect);
    //    }


    //window.display();

    //while (window.isOpen()) {}

    //return 0;

    srand(std::chrono::system_clock::now().time_since_epoch().count());

    auto model = model::RandomModel();
    auto game = game::Game(true, game::GameControlType::AI, 10, 10, 800, 800, model);

    game.SetSpeed(6);

    game.Start();

    return 0;
}