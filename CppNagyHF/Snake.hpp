#pragma once

#include "Direction.hpp"
#include "vector.hpp"
#include "position.hpp"
#ifndef JPORTA
#include <SFML/graphics.hpp>
#endif
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

		void UpdateHeadDirection(const model::Direction key);

		void Move(bool grow = false);
		bool WouldDieIfItMoved(int gameWidth, int gameHeight);
		bool WouldPickUpAppleIfItMoved(const cstd::Position& applePos);
#ifndef JPORTA
		void Render(sf::RenderWindow& window, int gameWidth, int gameHeight, int windowWidth, int windowHeight);
#endif
	};

}
