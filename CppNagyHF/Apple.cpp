#include "Apple.hpp"
#include "GameUtils.hpp"

namespace game {

	void Apple::PlaceAtRandom(int gameWidth, int gameHeight, const Snake& snake, bool placeInFrontOfSnake) {
	
        bool isApplePlacedOnSnake = false;

        size_t attempts = 0;

        do {
            isApplePlacedOnSnake = false;

            position = {
                rand() % gameWidth,
                rand() % gameHeight
            };

            for (const auto& bodyPart : snake.Body()) {
                if (placeInFrontOfSnake == false && (bodyPart.x == position.x || bodyPart.y == position.y)) {
                    isApplePlacedOnSnake = true;
                    break;
                }

                if (placeInFrontOfSnake && bodyPart.x == position.x && bodyPart.y == position.y) {
                    isApplePlacedOnSnake = true;
                    break;
                }
            }

            attempts++;

        } while (isApplePlacedOnSnake && attempts < MAX_ATTEMPTS_TO_PLACE_APPLE);

        if (attempts >= MAX_ATTEMPTS_TO_PLACE_APPLE) {
            std::cerr << "ERROR: could not place apple!" << std::endl;
            throw std::exception("Could not place apple!");
        }

	}

    bool Apple::CanPlace(int gameWidth, int gameHeight, const Snake& snake) {
        return snake.Body().size() < gameWidth * gameHeight;
    }

#ifndef CPORTA
    void Apple::Render(sf::RenderWindow& window, int gameWidth, int gameHeight, int windowWidth, int windowHeight) {

        sf::RectangleShape appleRect(Utils::GameUtils::GetPixelOfGamePosition(cstd::Position(1, 1), gameWidth, gameHeight, windowWidth, windowHeight)); // 1x1 square
        
        appleRect.setOrigin(0, 0);
        appleRect.setPosition(Utils::GameUtils::GetPixelOfGamePosition(position, gameWidth, gameHeight, windowWidth, windowHeight));
        appleRect.setFillColor(sf::Color::Red);
        appleRect.setOutlineThickness(0);

        window.draw(appleRect);
    }
#endif
}