#include "Snake.hpp"
#include "GameUtils.hpp"

#define RENDER_INPUT_SQUARES false

#define BODY_WIDTH 0.9

namespace game {

    void Snake::Move(bool grow) {

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

    void Snake::UpdateHeadDirection(const model::Direction direction) {

        switch (direction) {
        case model::UP:
            headDirection = cstd::Position(0, -1);
            break;
        case model::DOWN:
            headDirection = cstd::Position(0, 1);
            break;
        case model::RIGHT:
            headDirection = cstd::Position(1, 0);
            break;
        case model::LEFT:
            headDirection = cstd::Position(-1, 0);
            break;
        default:
            break;
        }
    }

#ifndef CPORTA
    void Snake::Render(sf::RenderWindow& window, int gameWidth, int gameHeight, int windowWidth, int windowHeight) {

#if RENDER_INPUT_SQUARES == true
        const cstd::Position& headPos = Body()[0];
        const cstd::Position& headDir = HeadDirection();

        auto size = Utils::GameUtils::GetPixelOfGamePosition(cstd::Position(1, 1), gameWidth, gameHeight, windowWidth, windowHeight);

        cstd::Position posInFront = headPos + headDir;
        cstd::Position posToRight = headPos + cstd::Position(-headDir.y, headDir.x);
        cstd::Position posToLeft = headPos + cstd::Position(headDir.y, -headDir.x);

        sf::RectangleShape testRect(size); // 1x1 square
        testRect.setOrigin(0, 0);
        testRect.setOutlineThickness(0);

        testRect.setPosition(Utils::GameUtils::GetPixelOfGamePosition(posInFront, gameWidth, gameHeight, windowWidth, windowHeight));
        testRect.setFillColor(sf::Color(100, 0, 0, 150));
        window.draw(testRect);

        testRect.setPosition(Utils::GameUtils::GetPixelOfGamePosition(posToRight, gameWidth, gameHeight, windowWidth, windowHeight));
        testRect.setFillColor(sf::Color(0, 100, 0, 150));
        window.draw(testRect);

        testRect.setPosition(Utils::GameUtils::GetPixelOfGamePosition(posToLeft, gameWidth, gameHeight, windowWidth, windowHeight));
        testRect.setFillColor(sf::Color(0, 0, 100, 150));
        window.draw(testRect);
#endif

        for (int i = 0; i < (int)body.size(); i++) {

            const auto& bodyPart = body[i];

            const double delta = (1.0 - BODY_WIDTH) / 2;

            auto unitSize = Utils::GameUtils::GetPixelOfGamePosition(cstd::Position(1, 1), gameWidth, gameHeight, windowWidth, windowHeight);

            const double deltaX = unitSize.x * delta;
            const double deltaY = unitSize.y * delta;
            const double widthX = unitSize.x * BODY_WIDTH;
            const double widthY = unitSize.y * BODY_WIDTH;

            auto bodyPos = Utils::GameUtils::GetPixelOfGamePosition(bodyPart, gameWidth, gameHeight, windowWidth, windowHeight);

            sf::RectangleShape bodyRect(sf::Vector2f(widthX, widthY)); // 1x1 square
            
            const sf::Color bodyColor = body.size() > 1 && i == 0 ? sf::Color(255, 255, 50, 255) : sf::Color(0, 250, 0, 255);

            bodyRect.setOrigin(0, 0);
            bodyRect.setPosition(bodyPos + sf::Vector2f(deltaX, deltaY));
            bodyRect.setFillColor(bodyColor);
            bodyRect.setOutlineThickness(0);

            window.draw(bodyRect);

            if (i > 0) {

                const auto& prevPos = body[i - 1];

                sf::RectangleShape connectionRectToPrev(sf::Vector2f(unitSize.x * BODY_WIDTH, unitSize.y * BODY_WIDTH)); // 1x1 square

                sf::Vector2f connectorPos = bodyPos;

                if (prevPos.x < bodyPart.x) {
                    connectorPos += sf::Vector2f(0, deltaY);
                    connectionRectToPrev.setSize(sf::Vector2f(deltaX, widthY));
                }
                else if (prevPos.x > bodyPart.x) {
                    connectorPos += sf::Vector2f(widthX + deltaX, deltaY);
                    connectionRectToPrev.setSize(sf::Vector2f(deltaX, widthY));
                } else if (prevPos.y < bodyPart.y) {
                    connectorPos += sf::Vector2f(deltaX, 0);
                    connectionRectToPrev.setSize(sf::Vector2f(widthX, deltaY));
                }
                else if (prevPos.y > bodyPart.y) {
                    connectorPos += sf::Vector2f(deltaX, widthY + deltaY);
                    connectionRectToPrev.setSize(sf::Vector2f(widthX, deltaY));
                }

                connectionRectToPrev.setOrigin(0, 0);
                connectionRectToPrev.setPosition(
                    connectorPos
                );
                connectionRectToPrev.setFillColor(bodyColor);
                connectionRectToPrev.setOutlineThickness(0);

                window.draw(connectionRectToPrev);    
            }

            if (i + 1 < (int)body.size()) {

                const auto& nextPos = body[i + 1];

                sf::RectangleShape connectionRectToNext(sf::Vector2f(unitSize.x * BODY_WIDTH, unitSize.y * BODY_WIDTH)); // 1x1 square

                sf::Vector2f connectorPos = bodyPos;


                if (nextPos.x < bodyPart.x) {
                    connectorPos += sf::Vector2f(0, deltaY);
                    connectionRectToNext.setSize(sf::Vector2f(deltaX, widthY));
                }
                else if (nextPos.x > bodyPart.x) {
                    connectorPos += sf::Vector2f(widthX + deltaX, deltaY);
                    connectionRectToNext.setSize(sf::Vector2f(deltaX, widthY));
                }
                else if (nextPos.y < bodyPart.y) {
                    connectorPos += sf::Vector2f(deltaX, 0);
                    connectionRectToNext.setSize(sf::Vector2f(widthX, deltaY));
                }
                else if (nextPos.y > bodyPart.y) {
                    connectorPos += sf::Vector2f(deltaX, widthY + deltaY);
                    connectionRectToNext.setSize(sf::Vector2f(widthX, deltaY));
                }

                connectionRectToNext.setOrigin(0, 0);
                connectionRectToNext.setPosition(
                    connectorPos
                );
                connectionRectToNext.setFillColor(bodyColor);
                connectionRectToNext.setOutlineThickness(0);

                window.draw(connectionRectToNext);
            }

        }
    }
#endif

}