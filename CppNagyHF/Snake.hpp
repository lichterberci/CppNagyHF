#pragma once

#include "vector.hpp"
#include "position.hpp"
#include <SFML/graphics.hpp>

namespace game {

	class Snake
	{
		cstd::Vector<cstd::Position> body;
		cstd::Position headDirection;

	public:
		Snake()
			: body(cstd::Vector<cstd::Position>())
		{}

		Snake(cstd::Position headPos) {
			body = cstd::Vector<cstd::Position>();
			body += headPos;
		}

		Snake(cstd::Position headPos, cstd::Position headDir) {
			body = cstd::Vector<cstd::Position>();
			body += headPos;

			headDirection = headDir;
		}

		cstd::Vector<cstd::Position>& Body() {
			return body;
		}

		const cstd::Vector<cstd::Position>& Body() const {
			return body;
		}

		cstd::Position& HeadDirection() {
			return headDirection;
		}

		const cstd::Position& HeadDirection() const {
			return headDirection;
		}

		void UpdateHeadDirection(cstd::Position newDirection) {
			headDirection = newDirection;
		}
		void UpdateHeadDirection(const sf::Keyboard::Key& key);

		void Move(bool grow = false);
		bool WouldDieIfItMoved(int gameWidth, int gameHeight);
		bool WouldPickUpAppleIfItMoved(const cstd::Position& applePos);
		void Render(sf::RenderWindow& window, int gameWidth, int gameHeight, int windowWidth, int windowHeight);
	};

}
