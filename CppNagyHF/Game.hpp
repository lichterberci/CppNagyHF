#pragma once

#include <SFML/Graphics.hpp>
#include "vector.hpp"
#include "position.hpp"
#include "ControllerModel.hpp"

namespace game {

	enum GameState {
		UNINITIALIZED, INITIALIZING, RUNNING, STOPPED
	};

	enum GameControlType {
		KEYBOARD, AI
	};

	class Game {
	private:			
		bool useUI;
		int width, height;
		cstd::Vector<cstd::Position> snake;
		cstd::Position headDirection;
		cstd::Position applePosition;
		unsigned int points;
		GameState gameState = GameState::UNINITIALIZED;
		GameControlType controlType;
		model::ControllerModel& controllerModel;

		void RunWithUI();
		void RunWithoutUI();

		void HandleResize(sf::Event event, sf::RenderWindow& window, sf::View& view);
		void HandleKeyPresses(sf::Event event, cstd::Vector<sf::Keyboard::Key> keyPresses);
		void Update(cstd::Vector<sf::Keyboard::Key> keyPresses, double timeDelta);
		void Render(sf::RenderWindow& window);
		
		void PlaceApple();
		void WouldSnakeDieIfItMoved(cstd::Position dir);
		void WouldSnakePickUpAppleIfItMoved(cstd::Position dir);
		void PickUpApple();
		void MoveSnake(cstd::Position dir, bool grow = false);

	public:
		Game(bool useUI = true, GameControlType controlType = KEYBOARD, int width = 640, int height = 480, model::ControllerModel& controllModel = *(model::ControllerModel*)nullptr)
			: useUI(useUI), controlType(controlType), width(width), height(height), controllerModel(controllerModel)
		{}

		void Start();

		void AttachModel(model::ControllerModel& model) {
			controllerModel = model;
		}

		void SetControlType(GameControlType controlType) {
			controlType = controlType;
		}
	};
}