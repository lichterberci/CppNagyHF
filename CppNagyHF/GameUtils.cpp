#include "GameUtils.hpp"

namespace Utils {

    sf::Vector2f GameUtils::GetPixelOfGamePosition(const cstd::Position& pos, int gameWidth, int gameHeight, int windowWidth, int windowHeight) {
        return sf::Vector2f(
            (float)pos.x * windowWidth / gameWidth,
            (float)pos.y * windowHeight / gameHeight
        );
    }

}
