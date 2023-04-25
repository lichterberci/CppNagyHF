#pragma once

#ifndef JPORTA
#include "SFML/Graphics.hpp"
#endif

#include "vector.hpp"
#include "position.hpp"

namespace Utils {

    class GameUtils {
    public:
#ifndef JPORTA
        static sf::Vector2f GetPixelOfGamePosition(const cstd::Position& pos, int gameWidth, int gameHeight, int windowWidth, int windowHeight);
#endif
    };

}
