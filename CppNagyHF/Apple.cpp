#include "Apple.hpp"
#include "GameUtils.hpp"

namespace game {

	void Apple::PlaceAtRandom(int gameWidth, int gameHeight, const Snake& snake, bool placeInFrontOfSnake) {
	
        bool isApplePlacedOnSnake = false;

        do {
            isApplePlacedOnSnake = false;

            position = {
                rand() % gameWidth,
                rand() % gameHeight
            };

            for (const auto& bodyPart : snake.Body()) {
                if (bodyPart.x == position.x || bodyPart.y == position.y) {
                    isApplePlacedOnSnake = true;
                    break;
                }
            }


        } while (isApplePlacedOnSnake);
	}

    bool Apple::CanPlace(int gameWidth, int gameHeight, const Snake& snake) {

        return snake.Body().size() < gameWidth * gameHeight;
    }

#ifndef JPORTA
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