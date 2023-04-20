#pragma once

#include "position.hpp"
#include "Snake.hpp"

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
#ifndef JPORTA
		void Render(sf::RenderWindow& window, int gameWidth, int gameHeight, int windowWidth, int windowHeight);
#endif
	};

}