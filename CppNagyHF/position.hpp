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

		constexpr Position& operator+= (const Position& rhs) {
			x += rhs.x;
			y += rhs.y;
			return *this;
		}

		constexpr Position& operator-= (const Position& rhs) {
			x -= rhs.x;
			y -= rhs.y;
			return *this;
		}
	};

	constexpr Position operator+ (Position lhs, const Position& rhs) {
		return lhs += rhs;
	}

	constexpr Position operator- (Position lhs, const Position& rhs) {
		return lhs -= rhs;
	}

	constexpr bool operator== (const Position& lhs, const Position& rhs) {
		return lhs.x == rhs.x && lhs.y == rhs.y;
	}

	constexpr bool operator!= (const Position& lhs, const Position& rhs) {
		return !(lhs == rhs);
	}

	std::ostream& operator<<(std::ostream& out, const Position& pos)
	{
		return out << "Position(" << pos.x << ", " << pos.y << ")";
	}
}