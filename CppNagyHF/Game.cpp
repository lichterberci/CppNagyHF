#include "Game.hpp"


namespace game {

    void Game::Start() {
        if (useUI)
            RunWithUI();
        else
            RunWithoutUI();
    }

	void Game::RunWithUI() {

        if (controlType == GameControlType::AI && &controllerModel == nullptr) {
            throw "No controller model attached!";
        }

        sf::RenderWindow window(sf::VideoMode(width, height), "Snake");
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
                    controllerModel.GetKeyPresses({
                        snake[0],
                        headDirection,
                        applePosition
                    }, keyPresses);
                }

                if (event.type == sf::Event::Resized)
                    HandleResize(event, window, view);

                if (event.type == sf::Event::Closed)
                    window.close();
            }

            window.clear();

            Update(keyPresses);

            Render(window);

            window.display();
        }
	}

    void Game::HandleResize(sf::Event event, sf::RenderWindow& window, sf::View &view) {
        width = event.size.width;
        height = event.size.height;

        // resize my view
        view.setSize({
                static_cast<float>(event.size.width),
                static_cast<float>(event.size.height)
            });

        window.setView(view);

        std::cout << "Resized to " << width << "x" << height << std::endl;
    }

    void Game::HandleKeyPresses(sf::Event event, cstd::Vector<sf::Keyboard::Key> keyPresses) {

    }


	void Game::Update(cstd::Vector<sf::Keyboard::Key> keyPresses, double timeDelta) {

	}

	void Game::Render(sf::RenderWindow& window) {
        
    }

}