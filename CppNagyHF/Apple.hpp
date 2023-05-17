#pragma once

#ifndef CPORTA
#include "SFML/Graphics.hpp"
#endif

#include "position.hpp"
#include "Snake.hpp"

#define MAX_ATTEMPTS_TO_PLACE_APPLE 200

namespace game {

	struct Apple {

		cstd::Position position;

		Apple()
			: position({0, 0})
		{}

		Apple(const cstd::Position& pos)
			: position(pos)
		{}

		bool CanPlace(int gameWidth, int gameHeight, const Snake& snake);
		void PlaceAtRandom(int gameWidth, int gameHeight, const Snake& snake, bool placeInFrontOfSnake = true);
#ifndef CPORTA
		void Render(sf::RenderWindow& window, int gameWidth, int gameHeight, int windowWidth, int windowHeight);
#endif
	};

}