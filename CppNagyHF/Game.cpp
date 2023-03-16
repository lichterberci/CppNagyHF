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
        
        InitializeSnake();
        PlaceApple();
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
                    UpdateHeadDirection(keyPresses[keyPresses.size() - 1]);
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

        const float sqrt2 = std::sqrtf(2);

        result.distancesToWall += gameWidth - snake[0].x;
        result.distancesToWall += sqrt2 * std::min(gameWidth - snake[0].x, gameHeight - snake[0].y);
        result.distancesToWall += gameHeight - snake[0].y;
        result.distancesToWall += sqrt2 * std::min(snake[0].x + 1, gameHeight - snake[0].y);
        result.distancesToWall += snake[0].x + 1;
        result.distancesToWall += sqrt2 * std::min(snake[0].x + 1, snake[0].y + 1);
        result.distancesToWall += snake[0].y + 1;
        result.distancesToWall += sqrt2 * std::min(gameWidth - snake[0].x, snake[0].y + 1);

        // distance to apple

        const int appleDeltaX = applePosition.x - snake[0].x;
        const int appleDeltaY = applePosition.y - snake[0].y;

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

        const cstd::Position& headPos = snake[0];

        for (int i = 1; i < snake.size(); i++) {

            int bodyDeltaX = snake[i].x - headPos.x;
            int bodyDeltaY = snake[i].y - headPos.y;

            if (bodyDeltaY == 0 && bodyDeltaX > 0)
                result.distancesToBody[0] = std::min<float>(result.distancesToBody[0], bodyDeltaX);

            if (bodyDeltaY == bodyDeltaX && bodyDeltaX > 0)
                result.distancesToBody[1] = std::min<float>(result.distancesToBody[1], bodyDeltaX * sqrt2);

            if (bodyDeltaX == 0 && bodyDeltaY > 0)
                result.distancesToBody[2] = std::min<float>(result.distancesToBody[2], bodyDeltaY);

            if (bodyDeltaY == -bodyDeltaX && bodyDeltaX < 0)
                result.distancesToBody[3] = std::min<float>(result.distancesToBody[3], bodyDeltaY * sqrt2);

            if (bodyDeltaY == 0 && bodyDeltaX < 0)
                result.distancesToBody[4] = std::min<float>(result.distancesToBody[4], -bodyDeltaX);

            if (bodyDeltaY == bodyDeltaX && bodyDeltaX < 0)
                result.distancesToBody[5] = std::min<float>(result.distancesToBody[5], -bodyDeltaY * sqrt2);

            if (bodyDeltaX == 0 && bodyDeltaY < 0)
                result.distancesToBody[6] = std::min<float>(result.distancesToBody[6], -bodyDeltaY);

            if (bodyDeltaY == -bodyDeltaX && bodyDeltaX > 0)
                result.distancesToBody[7] = std::min<float>(result.distancesToBody[7], bodyDeltaX * sqrt2);
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
            UpdateHeadDirection(keyPresses[keyPresses.size() - 1]);

        if (WouldSnakeDieIfItMoved()) {
            gameState = GameState::STOPPED;
            return;
        }

        if (WouldSnakePickUpAppleIfItMoved()) {
            PickUpApple();
            PlaceApple();
            MoveSnake(true);
        }
        else {
            MoveSnake(false);
        }
	}
    
    void Game::UpdateHeadDirection(const sf::Keyboard::Key& key) {

        switch (key) {
        case sf::Keyboard::Up:
            headDirection = cstd::Position(0, -1);
            break;
        case sf::Keyboard::Down:
            headDirection = cstd::Position(0, 1);
            break;
        case sf::Keyboard::Right:
            headDirection = cstd::Position(1, 0);
            break;
        case sf::Keyboard::Left:
            headDirection = cstd::Position(-1, 0);
            break;
        default:
            break;
        }
    }

    bool Game::WouldSnakeDieIfItMoved() {

        const auto newHead = snake[0] + headDirection;

        if (newHead.x < 0 || newHead.x >= gameWidth || newHead.y < 0 || newHead.y >= gameHeight)
            return true;

        for (int i = 1; i < snake.size(); i++) {
            if (newHead == snake[i])
                return true;
        }

        return false;
    }

    bool Game::WouldSnakePickUpAppleIfItMoved() {

        const auto newHead = snake[0] + headDirection;

        return newHead == applePosition;
    }

    void Game::PickUpApple() {

        points++;
    }

    void Game::PlaceApple() {

        bool isApplePlacedOnSnake = false;

        do {
            isApplePlacedOnSnake = false;

            applePosition = {
                rand() % gameWidth,
                rand() % gameHeight
            };

            for (const auto& bodyPart : snake)
                if (bodyPart == applePosition)
                    isApplePlacedOnSnake = true;

        } while (isApplePlacedOnSnake);
    }

    void Game::MoveSnake(bool grow) {

        const cstd::Position newHead = snake[0] + headDirection;
        
        snake.pushToFront(newHead);
        
        if (grow == false)
            snake.pop();
    }

    sf::Vector2f Game::GetPixelOfGamePosition(const cstd::Position& pos) const {
        return sf::Vector2f(
            (float)pos.x * windowWidth / gameWidth,
            (float)pos.y * windowHeight / gameHeight
        );
    }

    void Game::InitializeSnake() {
        snake = cstd::Vector<cstd::Position>();
        snake += cstd::Position(gameWidth / 2, gameHeight / 2);

        headDirection = cstd::Position(1, 0);
    }

	void Game::Render(sf::RenderWindow& window) {

        sf::RectangleShape appleRect(GetPixelOfGamePosition(cstd::Position(1, 1))); // 1x1 square
        appleRect.setOrigin(0, 0);
        appleRect.setPosition(GetPixelOfGamePosition(applePosition));
        appleRect.setFillColor(sf::Color::Red);
        appleRect.setOutlineThickness(0);
        window.draw(appleRect);

        for (const auto& bodyPart : snake) {
            sf::RectangleShape bodyRect(GetPixelOfGamePosition(cstd::Position(1, 1))); // 1x1 square
            bodyRect.setOrigin(0, 0);
            bodyRect.setPosition(GetPixelOfGamePosition(bodyPart));
            bodyRect.setFillColor(sf::Color::Green);
            bodyRect.setOutlineThickness(0);
            window.draw(bodyRect);
        }
    }

}