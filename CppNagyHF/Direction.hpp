#pragma once

#include <cstdint>
#ifndef JPORTA
#include <SFML/Graphics.hpp>
#endif
namespace model {

	enum Direction : int8_t {
		
		NONE = -1,
		RIGHT = 0,
		DOWN = 1,
		LEFT = 2,
		UP = 3

	};

#ifndef JPORTA

	inline Direction KeyToDirection(const sf::Keyboard::Key key) {
		switch (key) {
		case sf::Keyboard::D :
		case sf::Keyboard::Right :
			return Direction::RIGHT;
		case sf::Keyboard::S:
		case sf::Keyboard::Down:
			return Direction::DOWN;
		case sf::Keyboard::A:
		case sf::Keyboard::Left:
			return Direction::LEFT;
		case sf::Keyboard::W:
		case sf::Keyboard::Up:
			return Direction::UP;
		}
	}

#endif

}