#pragma once

#include <ostream>

namespace cstd {

	struct Position {

		int x, y;

		Position() 
			: x(0), y(0) 
		{}

		Position(int _x, int _y)
			: x(_x), y(_y)
		{}

		Position(const Position& other)
			: x(other.x), y(other.y)
		{}

		Position(Position&& other) noexcept
			: x(other.x), y(other.y)
		{
			other.x = 0;
			other.y = 0;
		}

		Position& operator= (const Position& other) {
			x = other.x;
			y = other.y;
			return *this;
		}

		Position& operator= (Position&& other) {
			x = other.x;
			y = other.y;
			other.x = 0;
			other.y = 0;
			return *this;
		}

		Position& operator+= (const Position& rhs) {
			x += rhs.x;
			y += rhs.y;
			return *this;
		}

		Position& operator-= (const Position& rhs) {
			x -= rhs.x;
			y -= rhs.y;
			return *this;
		}

		friend std::ostream& operator<<(std::ostream& out, const Position& pos)
		{
			return out << "Position(" << pos.x << ", " << pos.y << ")";
		}

		Position operator+ (const Position& rhs) const {
			return Position(x + rhs.x, y + rhs.y);
		}

		Position operator- (const Position& rhs) const {
			return Position(x - rhs.x, y - rhs.y);
		}
				
	};

	constexpr bool operator== (const Position& lhs, const Position& rhs) {
		return lhs.x == rhs.x && lhs.y == rhs.y;
	}

	constexpr bool operator!= (const Position& lhs, const Position& rhs) {
		return !(lhs == rhs);
	}
}