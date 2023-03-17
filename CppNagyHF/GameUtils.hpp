#pragma once

#include "vector.hpp"
#include "position.hpp"
#include <SFML/Graphics.hpp>

namespace Utils {

    class GameUtils {
    public:
        static sf::Vector2f GetPixelOfGamePosition(const cstd::Position& pos, int gameWidth, int gameHeight, int windowWidth, int windowHeight);
    };

}
