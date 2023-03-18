// CppNagyHF.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <SFML/Graphics.hpp>
#include "vector.hpp"
#include "Game.hpp"
#include <chrono>
#include "RandomModel.hpp"
#include "NeatModel.hpp"
#include <future>

game::GameReport PlayGame() {

    auto neat = model::NeatModel(NUM_SENSORS, NUM_OUTPUTS, model::LReLU());

    auto game = game::Game(false, game::GameControlType::AI, 10, 10, 800, 800);

    game.AttachModel(neat);

    game.Start();

    return game.GenerateReport();
}

int main()
{

    srand(std::chrono::system_clock::now().time_since_epoch().count());

    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::future<game::GameReport>> futures;
    std::vector<game::GameReport> reports;

    for (int i = 0; i < 1000; i++) {

        //std::cout << i << "/" << 100 << std::endl;

        futures.push_back(std::async(std::launch::async, PlayGame));

        //reports.push_back(PlayGame());
    }

    for (auto& future : futures)
        reports.push_back(future.get());

    auto end = std::chrono::high_resolution_clock::now();

    std::cout << std::chrono::time_point_cast<std::chrono::seconds>(end).time_since_epoch().count() - std::chrono::time_point_cast<std::chrono::seconds>(start).time_since_epoch().count() << "s" << std::endl;

    for (const auto& report : reports)
        std::cout << report << std::endl;

    return 0;
}