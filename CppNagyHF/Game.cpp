#include "Game.hpp"
#include <math.h>
#include <exception>

#define SNAKE_SIGHT_DISTANCE 9999

namespace game {

    void Game::Start() {

        deltaClock.restart();

        if (useUI)
            RunWithUI();
        else
            RunWithoutUI();
    }

	void Game::RunWithUI() {

        if (controlType == GameControlType::AI && (&controllerModel) == nullptr) {
            throw std::exception("No controller model attached!");
        }

        sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Snake");
        sf::View view = window.getDefaultView();
        
        gameState = GameState::RUNNING;

        while (window.isOpen())
        {
            sf::Event event;

            cstd::Vector<sf::Keyboard::Key> keyPresses;

            while (window.pollEvent(event))
            {

                if (controlType == GameControlType::KEYBOARD) {
                    if (event.type == sf::Event::KeyPressed)
                        HandleKeyPresses(event, keyPresses);
                }

                if (controlType == GameControlType::AI) {
                    controllerModel.GetKeyPresses(CalculateModelParams(), keyPresses);
                }

                if (event.type == sf::Event::Resized)
                    HandleResize(event, window, view);

                if (event.type == sf::Event::Closed)
                    window.close();
            }

            window.clear();

            auto deltaTime = deltaClock.restart();

            Update(keyPresses, deltaTime.asSeconds());

            Render(window);

            window.display();
        }
	}

    void Game::RunWithoutUI() {
        
    }

    model::ModelParams Game::CalculateModelParams() {

        model::ModelParams result;

        /*
        
        Directions:

             5      6      7

                \   |   /
                    
             4  -   H   -  0

                /   |   \

             3      2      1
        
        */

        // distance to wall

        const float sqrt2Over2 = std::sqrtf(2) / 2;

        result.distancesToWall += gameWidth - snake[0].x;
        result.distancesToWall += sqrt2Over2 * std::min(gameWidth - snake[0].x, gameHeight - snake[0].y);
        result.distancesToWall += gameHeight - snake[0].y;
        result.distancesToWall += sqrt2Over2 * std::min(snake[0].x + 1, gameHeight - snake[0].y);
        result.distancesToWall += snake[0].x + 1;
        result.distancesToWall += sqrt2Over2 * std::min(snake[0].x + 1, snake[0].y + 1);
        result.distancesToWall += snake[0].y + 1;
        result.distancesToWall += sqrt2Over2 * std::min(gameWidth - snake[0].x, snake[0].y + 1);

        // distance to apple

        const int appleDeltaX = applePosition.x - snake[0].x;
        const int appleDeltaY = applePosition.y - snake[0].y;

        result.distancesToApple += appleDeltaY == 0 && appleDeltaX > 0 ? appleDeltaX : -1;
        result.distancesToApple += appleDeltaY == appleDeltaX && appleDeltaX > 0 ? appleDeltaX : -1;
        result.distancesToApple += appleDeltaX == 0 && appleDeltaY > 0 ? appleDeltaY : -1;
        result.distancesToApple += appleDeltaY == -appleDeltaX && appleDeltaX < 0 ? -appleDeltaX : -1;
        result.distancesToApple += appleDeltaY == 0 && appleDeltaX < 0 ? -appleDeltaX : -1;
        result.distancesToApple += appleDeltaY == appleDeltaX && appleDeltaX < 0 ? -appleDeltaX : -1;
        result.distancesToApple += appleDeltaX == 0 && appleDeltaY < 0 ? -appleDeltaY : -1;
        result.distancesToApple += appleDeltaY == -appleDeltaX && appleDeltaX > 0 ? appleDeltaX : -1;

        // distance to body

        for (int i = 0; i < 8; i++)
            result.distancesToBody += SNAKE_SIGHT_DISTANCE;

        const cstd::Position& headPos = snake[0];

        for (int i = 1; i < snake.size(); i++) {

            int bodyDeltaX = snake[i].x - headPos.x;
            int bodyDeltaY = snake[i].y - headPos.y;

            if (bodyDeltaY == 0 && bodyDeltaX > 0)
                result.distancesToBody[0] = std::min((int)result.distancesToBody[0], bodyDeltaX);

            if (bodyDeltaY == bodyDeltaX && bodyDeltaX > 0)
                result.distancesToBody[1] = std::min((int)result.distancesToBody[1], bodyDeltaX);

            if (bodyDeltaX == 0 && bodyDeltaY > 0)
                result.distancesToBody[2] = std::min((int)result.distancesToBody[2], bodyDeltaY);

            if (bodyDeltaY == -bodyDeltaX && bodyDeltaX < 0)
                result.distancesToBody[3] = std::min((int)result.distancesToBody[3], bodyDeltaY);

            if (bodyDeltaY == 0 && bodyDeltaX < 0)
                result.distancesToBody[4] = std::min((int)result.distancesToBody[4], -bodyDeltaX);

            if (bodyDeltaY == bodyDeltaX && bodyDeltaX < 0)
                result.distancesToBody[5] = std::min((int)result.distancesToBody[5], -bodyDeltaY);

            if (bodyDeltaX == 0 && bodyDeltaY < 0)
                result.distancesToBody[6] = std::min((int)result.distancesToBody[6], -bodyDeltaY);

            if (bodyDeltaY == -bodyDeltaX && bodyDeltaX > 0)
                result.distancesToBody[7] = std::min((int)result.distancesToBody[7], bodyDeltaX);
        }

        return result;
    }

    void Game::HandleResize(sf::Event event, sf::RenderWindow& window, sf::View &view) {
        windowWidth = event.size.width;
        windowHeight = event.size.height;

        // resize my view
        view.setSize({
                static_cast<float>(event.size.width),
                static_cast<float>(event.size.height)
            });

        window.setView(view);

        std::cout << "Resized to " << windowWidth << "x" << windowHeight << std::endl;
    }

    void Game::HandleKeyPresses(sf::Event event, cstd::Vector<sf::Keyboard::Key> keyPresses) {

    }


	void Game::Update(cstd::Vector<sf::Keyboard::Key> keyPresses, float deltaTime) {

	}

	void Game::Render(sf::RenderWindow& window) {
        
    }

}