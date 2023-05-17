#include "Game.hpp"
#include <math.h>
#include <exception>
#include "Direction.hpp"

# define M_PI           3.14159265358979323846  /* pi */

#define CALC_AND_PRINT_MODEL_PARAMS false

namespace game {

    model::RandomModel Game::s_defaultModel = model::RandomModel();

    void Game::Start() {

#ifndef CPORTA
        deltaClock.restart();

        if (useUI)
            RunWithUI();
        else
            RunWithoutUI();
#else
        RunWithoutUI();
#endif
    }

#ifndef CPORTA
	void Game::RunWithUI() {

        if (controlType == GameControlType::AI && p_controllerModel == nullptr) {
            std::cout << "No controller model attached!" << std::endl;
            throw std::exception("No controller model attached!");
        }

        sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Snake");
        sf::View view = window.getDefaultView();
        
        snake = Snake({ gameWidth / 2, gameHeight / 2 });
        apple.PlaceAtRandom(gameWidth, gameHeight, snake, placeFirstAppleInFrontOfSnake);
        points = 0;
        
        Render(window);

        window.display();

        // wait until SPACE button is pressed
        bool waitingForKeyPressToStart = true;

        while (waitingForKeyPressToStart) {
            sf::Event event;
            cstd::Vector<model::Direction> keyPresses;
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

            cstd::Vector<model::Direction> keyPresses;

            // wait
            while (deltaClock.getElapsedTime().asSeconds() < 1.0 / snakeMovesPerSec) {}
             
#if CALC_AND_PRINT_MODEL_PARAMS == true
            const auto params = CalculateModelParams();
            std::clog << (params.blockToLeft == 1 ? "(<-)" : "(  )") << " " 
                << (params.blockInFront == 1 ? "(A)" : "( )") << " " 
                << (params.blockToRight == 1 ? "(->)" : "(  )") << " "
                << "(" << params.angleToAppleOnLeft << " " << params.angleToAppleOnRight << ")"
                << std::endl;
#endif

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

            if (controlType == GameControlType::AI) {
                p_controllerModel->GetKeyPresses(CalculateModelParams(), keyPresses);
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

    bool Game::HandleKeyPresses(sf::Event event, cstd::Vector<model::Direction>& out_keyPresses) {
        
        bool wasThereValidKeypress = false;

        switch (event.key.code) {
            case sf::Keyboard::Up: 
            case sf::Keyboard::W:
                out_keyPresses.push(model::KeyToDirection(sf::Keyboard::Up));
                wasThereValidKeypress = true;
                break;
            case sf::Keyboard::Down: 
            case sf::Keyboard::S:
                out_keyPresses.push(model::KeyToDirection(sf::Keyboard::Down));
                wasThereValidKeypress = true;
                break;
            case sf::Keyboard::Right: 
            case sf::Keyboard::D:
                out_keyPresses.push(model::KeyToDirection(sf::Keyboard::Right));
                wasThereValidKeypress = true;
                break;
            case sf::Keyboard::Left: 
            case sf::Keyboard::A:
                out_keyPresses.push(model::KeyToDirection(sf::Keyboard::Left));
                wasThereValidKeypress = true;
                break;
            default:
                break;
        }

        return wasThereValidKeypress;
    }
#endif

	void Game::Update(cstd::Vector<model::Direction> keyPresses) {
       
        if (keyPresses.size() > 0)
            snake.UpdateHeadDirection(keyPresses.last());

        if (snake.WouldDieIfItMoved(gameWidth, gameHeight)) {

            if (useUI)
                std::cout << "Snake hit the wall or its body, game is lost!" << std::endl;

            gameState = GameState::STOPPED;
            return;
        }

        if (numIdleSteps >= numMaxIdleSteps) {

            if (useUI)
                std::cout << "Game limit reached, session stopped!" << std::endl;

            gameState = GameState::STOPPED;
            return;
        }

        if (snake.WouldPickUpAppleIfItMoved(apple.position)) {            

            points++;
            snake.Move(true);

            if (apple.CanPlace(gameWidth, gameHeight, snake) == false) {

                if (useUI)
                    std::cout << "No more room for apples, game is won!" << std::endl;

                gameState = GameState::STOPPED;
                return;
            }

            apple.PlaceAtRandom(gameWidth, gameHeight, snake);

            numIdleSteps = 0;

        } else {

            snake.Move(false);

            numIdleSteps++;
        }

        numSteps++;
	}   

#ifndef CPORTA

	void Game::Render(sf::RenderWindow& window) {

        apple.Render(window, gameWidth, gameHeight, windowWidth, windowHeight);
        snake.Render(window, gameWidth, gameHeight, windowWidth, windowHeight);
    }

#endif

    void Game::RunWithoutUI() {
        
        if (controlType == KEYBOARD) {
            std::cout << "ERROR: you are trying playing with keyboards without UI!" << std::endl;
            throw std::exception("You are trying playing with keyboards without UI!");
        }

        snake = Snake({ gameWidth / 2, gameHeight / 2 }, { 1, 0 });
        apple.PlaceAtRandom(gameWidth, gameHeight, snake, placeFirstAppleInFrontOfSnake);
        points = 0;
        
        gameState = GameState::RUNNING;

        while (gameState == GameState::RUNNING) {

            cstd::Vector<model::Direction> keyPresses;

            p_controllerModel->GetKeyPresses(CalculateModelParams(), keyPresses);

            Update(keyPresses);
        }
    }

    model::ModelParams Game::CalculateModelParams() {

        model::ModelParams result;

#if USE_RELATIVE_DIRECTION != true
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

        //// normalize
        //for (auto& d : result.distancesToWall)
        //    d /= SNAKE_SIGHT_DISTANCE;

        // distance to apple

        const int appleDeltaX = apple.position.x - snakeBody[0].x;
        const int appleDeltaY = apple.position.y - snakeBody[0].y;

        const int APPLE_NOT_SEEN = -1;

        result.distancesToApple += appleDeltaY == 0 && appleDeltaX > 0 ? appleDeltaX : APPLE_NOT_SEEN;
        result.distancesToApple += appleDeltaY == appleDeltaX && appleDeltaX > 0 ? appleDeltaX * sqrt2 : APPLE_NOT_SEEN;
        result.distancesToApple += appleDeltaX == 0 && appleDeltaY > 0 ? appleDeltaY : APPLE_NOT_SEEN;
        result.distancesToApple += appleDeltaY == -appleDeltaX && appleDeltaX < 0 ? -appleDeltaX * sqrt2 : APPLE_NOT_SEEN;
        result.distancesToApple += appleDeltaY == 0 && appleDeltaX < 0 ? -appleDeltaX : APPLE_NOT_SEEN;
        result.distancesToApple += appleDeltaY == appleDeltaX && appleDeltaX < 0 ? -appleDeltaX * sqrt2 : APPLE_NOT_SEEN;
        result.distancesToApple += appleDeltaX == 0 && appleDeltaY < 0 ? -appleDeltaY : APPLE_NOT_SEEN;
        result.distancesToApple += appleDeltaY == -appleDeltaX && appleDeltaX > 0 ? appleDeltaX * sqrt2 : APPLE_NOT_SEEN;

        // normalize
        //for (auto& d : result.distancesToApple)
        //    d /= SNAKE_SIGHT_DISTANCE;

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

        for (double& distanceToBody : result.distancesToBody)
            if (distanceToBody == SNAKE_SIGHT_DISTANCE)
                distanceToBody = -1;

        // normalize
        //for (auto& d : result.distancesToBody)
        //    d /= SNAKE_SIGHT_DISTANCE;

#else

        const cstd::Position& headPos = snake.Body()[0];
        const cstd::Position& headDir = snake.HeadDirection();
        const cstd::Position& applePos = apple.position;

        double normalizedAngleToApple;

        if (applePos.x >= headPos.x) {
            // it is to the right of us
            normalizedAngleToApple = -atan2(applePos.y - headPos.y, applePos.x - headPos.x) / M_PI;
        } 
        else {
            // flip the plane around the head of the snake
            normalizedAngleToApple = -atan2(headPos.y - applePos.y, headPos.x - applePos.x) / M_PI;
            normalizedAngleToApple -= 1;

            if (normalizedAngleToApple <= -1)
                normalizedAngleToApple += 2;
        }

        if (headDir.x == 1 && headDir.y == 0) {
            normalizedAngleToApple += 0;
            result.currentDirection = 0;
        }
        else if (headDir.x == 0 && headDir.y == 1) {
            normalizedAngleToApple += 0.5;
            result.currentDirection = 1;
        }
        else if (headDir.x == 0 && headDir.y == -1) {
            normalizedAngleToApple -= 0.5;
            result.currentDirection = 3;
        }
        else {
            normalizedAngleToApple += 1;

            result.currentDirection = 2;
        }

        while (normalizedAngleToApple >= 1)
            normalizedAngleToApple -= 2;

        while (normalizedAngleToApple < -1)
            normalizedAngleToApple += 2;

        // negative = on right, positive = on left
        result.angleToAppleOnRight = normalizedAngleToApple < 0 ? -normalizedAngleToApple : 0;
        result.angleToAppleOnLeft = normalizedAngleToApple >= 0 ? normalizedAngleToApple : 0;

        cstd::Position posInFront = headPos + headDir;
        cstd::Position posToRight = headPos + cstd::Position(-headDir.y, headDir.x);
        cstd::Position posToLeft = headPos + cstd::Position(headDir.y, -headDir.x);

        result.blockInFront = 0;
        result.blockToLeft = 0;
        result.blockToRight = 0;

        for (const auto& bodyPart : snake.Body()) {
            if (bodyPart == posInFront)
                result.blockInFront = 1;
            else if (bodyPart == posToRight)
                result.blockToRight = 1;
            else if (bodyPart == posToLeft)
                result.blockToLeft = 1;
        }

        if (posInFront.y >= gameHeight || posInFront.x >= gameWidth || posInFront.x < 0 || posInFront.y < 0)
            result.blockInFront = 1;

        if (posToLeft.y >= gameHeight || posToLeft.x >= gameWidth || posToLeft.x < 0 || posToLeft.y < 0)
            result.blockToLeft = 1;
        
        if (posToRight.y >= gameHeight || posToRight.x >= gameWidth || posToRight.x < 0 || posToRight.y < 0)
            result.blockToRight = 1;
#endif
        return result;
    }

    GameReport Game::GenerateReport() const {
        return GameReport(points == snake.Body().size(), points, numSteps);
    }

}