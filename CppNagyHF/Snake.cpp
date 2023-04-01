#include "Snake.hpp"
#include "GameUtils.hpp"

namespace game {

    void Snake::Move(bool grow) {

        if (grow)
            int a = 0;

        const auto newHead = body[0] + headDirection;

        body.pushToFront(newHead);

        if (grow == false)
            body.pop();
    }

    bool Snake::WouldDieIfItMoved(int gameWidth, int gameHeight) {

        const cstd::Position newHead = body[0] + headDirection;

        if (newHead.x < 0 || newHead.x >= gameWidth || newHead.y < 0 || newHead.y >= gameHeight)
            return true;

        for (int i = 1; i < body.size(); i++) {
            if (newHead == body[i])
                return true;
        }

        return false;
    }

    bool Snake::WouldPickUpAppleIfItMoved(const cstd::Position& applePos) {

        const auto newHead = body[0] + headDirection;

        return newHead == applePos;
    }

    void Snake::UpdateHeadDirection(const sf::Keyboard::Key& key) {

        switch (key) {
        case sf::Keyboard::Up:
            headDirection = cstd::Position(0, -1);
            break;
        case sf::Keyboard::Down:
            headDirection = cstd::Position(0, 1);
            break;
        case sf::Keyboard::Right:
            headDirection = cstd::Position(1, 0);
            break;
        case sf::Keyboard::Left:
            headDirection = cstd::Position(-1, 0);
            break;
        default:
            break;
        }
    }

    void Snake::Render(sf::RenderWindow& window, int gameWidth, int gameHeight, int windowWidth, int windowHeight) {

        for (const auto& bodyPart : body) {

            if (bodyPart == cstd::Position(0, 0))
                int a = 0;

            auto size = Utils::GameUtils::GetPixelOfGamePosition(cstd::Position(1, 1), gameWidth, gameHeight, windowWidth, windowHeight);

            sf::RectangleShape bodyRect(size); // 1x1 square
            
            bodyRect.setOrigin(0, 0);
            bodyRect.setPosition(Utils::GameUtils::GetPixelOfGamePosition(bodyPart, gameWidth, gameHeight, windowWidth, windowHeight));
            bodyRect.setFillColor(sf::Color::Green);
            bodyRect.setOutlineThickness(0);

            window.draw(bodyRect);
        }
    }

}