// CppNagyHF.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <SFML/Graphics.hpp>
#include "vector.hpp"
#include "Game.hpp"

int main()
{
    sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    sf::View view = window.getDefaultView();

    auto game = game::Game();

    while (window.isOpen())
    {
        sf::Event event;
        cstd::Vector<sf::Keyboard::Key> keyPresses;

        while (window.pollEvent(event))
        {

            if (event.type == sf::Event::KeyPressed) {
                keyPresses.push(event.key.code);
            }

            if (event.type == sf::Event::Resized) {
                // resize my view
                view.setSize({
                        static_cast<float>(event.size.width),
                        static_cast<float>(event.size.height)
                    });

                window.setView(view);

                std::cout << "Resized to " << event.size.width << "x" << event.size.height << std::endl;
            }


            if (event.type == sf::Event::Closed)
                window.close();
        }

        game.Update(keyPresses);

        window.clear();
        window.draw(shape); 
        window.display();
    }

    return 0;
}