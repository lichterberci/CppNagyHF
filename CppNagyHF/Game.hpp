#pragma once

#include <SFML/Graphics.hpp>
#include "vector.hpp"
#include "position.hpp"

namespace game {

	enum GameState {
		UNINITIALIZED, INITIALIZING, RUNNING, STOPPED
	};

	class Game {
	private:			
		sf::Window& window;
		bool useUI;
		int width, height;
		cstd::Vector<cstd::Position> snake;
		cstd::Position apple;
		unsigned int points;
		GameState gameState = GameState::UNINITIALIZED;

		void PlaceApple();
		void WouldSnakeDieIfItMoved(cstd::Position dir);
		void WouldSnakePickUpAppleIfItMoved(cstd::Position dir);
		void PickUpApple();
		void MoveSnake(cstd::Position dir, bool grow = false);

	public:
		Game(bool useUI = true, int width = 30, int height = 20, sf::Window& window = *(sf::Window*)nullptr)
			: window(window), useUI(useUI), width(width), height(height)
		{}

		void Start();
		void Update(cstd::Vector<sf::Keyboard::Key> keyPresses);
		void Render();
	};
}