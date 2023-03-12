#include "Game.hpp"
#include <math.h>
#include <exception>

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

        float newDistance = -1;

        for (int x = snake[0].x + 1; x < gameWidth; x++)
            if ()


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