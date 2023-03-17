#include "Game.hpp"
#include <math.h>
#include <exception>

namespace game {

    model::RandomModel Game::s_defaultModel = model::RandomModel();

    void Game::Start() {

        deltaClock.restart();

        if (useUI)
            RunWithUI();
        else
            RunWithoutUI();
    }

	void Game::RunWithUI() {

        if (controlType == GameControlType::AI && (&controllerModel) == nullptr) {
            std::cout << controllerModel.Predict(model::ModelParams()) << std::endl;
            std::cout << "No controller model attached!" << std::endl;
            throw std::exception("No controller model attached!");
        }

        sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Snake");
        sf::View view = window.getDefaultView();
        
        snake = Snake({ gameWidth / 2, gameHeight / 2 });
        apple.PlaceAtRandom(gameWidth, gameHeight, snake);
        points = 0;
        
        Render(window);

        window.display();

        // wait until SPACE button is pressed
        bool waitingForKeyPressToStart = true;

        while (waitingForKeyPressToStart) {
            sf::Event event;
            cstd::Vector<sf::Keyboard::Key> keyPresses;
            while (window.pollEvent(event)) {
                if (event.KeyPressed && HandleKeyPresses(event, keyPresses)) {
                    snake.UpdateHeadDirection(keyPresses[keyPresses.size() - 1]);
                    waitingForKeyPressToStart = false;
                }
            }
        }

        gameState = GameState::RUNNING;

        while (window.isOpen() && gameState == GameState::RUNNING)
        {
            sf::Event event;

            cstd::Vector<sf::Keyboard::Key> keyPresses;

            // wait
            while (deltaClock.getElapsedTime().asSeconds() < 1.0 / snakeMovesPerSec) {}

            if (controlType == GameControlType::KEYBOARD) {
                while (window.pollEvent(event))
                {

                    if (controlType == GameControlType::KEYBOARD) {
                        if (event.type == sf::Event::KeyPressed)
                            HandleKeyPresses(event, keyPresses);
                    }

                    if (event.type == sf::Event::Resized)
                        HandleResize(event, window, view);

                    if (event.type == sf::Event::Closed)
                        window.close();
                }
            }
            else if (controlType == GameControlType::AI) {
                controllerModel.GetKeyPresses(CalculateModelParams(), keyPresses);
            }

            deltaClock.restart();

            Update(keyPresses);

            window.clear();

            Render(window);

            window.display();
        }
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

    bool Game::HandleKeyPresses(sf::Event event, cstd::Vector<sf::Keyboard::Key>& out_keyPresses) {
        switch (event.key.code) {
            case sf::Keyboard::Up: 
            case sf::Keyboard::W:
                out_keyPresses.push(sf::Keyboard::Up);
                break;
            case sf::Keyboard::Down: 
            case sf::Keyboard::S:
                out_keyPresses.push(sf::Keyboard::Down);
                break;
            case sf::Keyboard::Right: 
            case sf::Keyboard::D:
                out_keyPresses.push(sf::Keyboard::Right);
                break;
            case sf::Keyboard::Left: 
            case sf::Keyboard::A:
                out_keyPresses.push(sf::Keyboard::Left);
                break;
            default:
                return false;
        }

        return true;
    }

	void Game::Update(cstd::Vector<sf::Keyboard::Key> keyPresses) {
       
        if (keyPresses.size() > 0)
            snake.UpdateHeadDirection(keyPresses[keyPresses.size() - 1]);

        if (snake.WouldDieIfItMoved(gameWidth, gameHeight)) {
            std::cout << "Snake hit the wall, game is lost!" << std::endl;
            gameState = GameState::STOPPED;
            return;
        }

        if (snake.WouldPickUpAppleIfItMoved(apple.position)) {            

            points++;
            snake.Move(true);

            if (apple.CanPlace(gameWidth, gameHeight, snake) == false) {
                std::cout << "No more room for apples, game is won!" << std::endl;
                gameState = GameState::STOPPED;
                return;
            }

            apple.PlaceAtRandom(gameWidth, gameHeight, snake);
        }
        else {
            snake.Move(false);
        }
	}   

	void Game::Render(sf::RenderWindow& window) {

        apple.Render(window, gameWidth, gameHeight, windowWidth, windowHeight);
        snake.Render(window, gameWidth, gameHeight, windowWidth, windowHeight);
    }

    void Game::RunWithoutUI() {
        return;
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

        const auto& snakeBody = snake.Body();

        const float sqrt2 = std::sqrtf(2);

        result.distancesToWall += gameWidth - snakeBody[0].x;
        result.distancesToWall += sqrt2 * std::min(gameWidth - snakeBody[0].x, gameHeight - snakeBody[0].y);
        result.distancesToWall += gameHeight - snakeBody[0].y;
        result.distancesToWall += sqrt2 * std::min(snakeBody[0].x + 1, gameHeight - snakeBody[0].y);
        result.distancesToWall += snakeBody[0].x + 1;
        result.distancesToWall += sqrt2 * std::min(snakeBody[0].x + 1, snakeBody[0].y + 1);
        result.distancesToWall += snakeBody[0].y + 1;
        result.distancesToWall += sqrt2 * std::min(gameWidth - snakeBody[0].x, snakeBody[0].y + 1);

        // distance to apple

        const int appleDeltaX = apple.position.x - snakeBody[0].x;
        const int appleDeltaY = apple.position.y - snakeBody[0].y;

        const int APPLE_NOT_SEEN = SNAKE_SIGHT_DISTANCE;

        result.distancesToApple += appleDeltaY == 0 && appleDeltaX > 0 ? appleDeltaX : APPLE_NOT_SEEN;
        result.distancesToApple += appleDeltaY == appleDeltaX && appleDeltaX > 0 ? appleDeltaX * sqrt2 : APPLE_NOT_SEEN;
        result.distancesToApple += appleDeltaX == 0 && appleDeltaY > 0 ? appleDeltaY : APPLE_NOT_SEEN;
        result.distancesToApple += appleDeltaY == -appleDeltaX && appleDeltaX < 0 ? -appleDeltaX * sqrt2 : APPLE_NOT_SEEN;
        result.distancesToApple += appleDeltaY == 0 && appleDeltaX < 0 ? -appleDeltaX : APPLE_NOT_SEEN;
        result.distancesToApple += appleDeltaY == appleDeltaX && appleDeltaX < 0 ? -appleDeltaX * sqrt2 : APPLE_NOT_SEEN;
        result.distancesToApple += appleDeltaX == 0 && appleDeltaY < 0 ? -appleDeltaY : APPLE_NOT_SEEN;
        result.distancesToApple += appleDeltaY == -appleDeltaX && appleDeltaX > 0 ? appleDeltaX * sqrt2 : APPLE_NOT_SEEN;

        // distance to body

        for (int i = 0; i < 8; i++)
            result.distancesToBody += SNAKE_SIGHT_DISTANCE;

        const cstd::Position& headPos = snakeBody[0];

        for (int i = 1; i < snakeBody.size(); i++) {

            int bodyDeltaX = snakeBody[i].x - headPos.x;
            int bodyDeltaY = snakeBody[i].y - headPos.y;

            if (bodyDeltaY == 0 && bodyDeltaX > 0)
                result.distancesToBody[0] = std::min<double>(result.distancesToBody[0], bodyDeltaX);

            if (bodyDeltaY == bodyDeltaX && bodyDeltaX > 0)
                result.distancesToBody[1] = std::min<double>(result.distancesToBody[1], bodyDeltaX * sqrt2);

            if (bodyDeltaX == 0 && bodyDeltaY > 0)
                result.distancesToBody[2] = std::min<double>(result.distancesToBody[2], bodyDeltaY);

            if (bodyDeltaY == -bodyDeltaX && bodyDeltaX < 0)
                result.distancesToBody[3] = std::min<double>(result.distancesToBody[3], bodyDeltaY * sqrt2);

            if (bodyDeltaY == 0 && bodyDeltaX < 0)
                result.distancesToBody[4] = std::min<double>(result.distancesToBody[4], -bodyDeltaX);

            if (bodyDeltaY == bodyDeltaX && bodyDeltaX < 0)
                result.distancesToBody[5] = std::min<double>(result.distancesToBody[5], -bodyDeltaY * sqrt2);

            if (bodyDeltaX == 0 && bodyDeltaY < 0)
                result.distancesToBody[6] = std::min<double>(result.distancesToBody[6], -bodyDeltaY);

            if (bodyDeltaY == -bodyDeltaX && bodyDeltaX > 0)
                result.distancesToBody[7] = std::min<double>(result.distancesToBody[7], bodyDeltaX * sqrt2);
        }

        return result;
    }


}