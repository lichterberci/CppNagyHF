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
		int windowWidth, windowHeight;
		int gameWidth, gameHeight;
		GameState gameState = GameState::UNINITIALIZED;
		GameControlType controlType;
		model::ControllerModel& controllerModel;

		cstd::Vector<cstd::Position> snake;
		cstd::Position headDirection;
		cstd::Position applePosition;
		unsigned int points;

		sf::Clock deltaClock;

		void RunWithUI();
		void RunWithoutUI();

		void HandleResize(sf::Event event, sf::RenderWindow& window, sf::View& view);
		void HandleKeyPresses(sf::Event event, cstd::Vector<sf::Keyboard::Key> keyPresses);
		void Update(cstd::Vector<sf::Keyboard::Key> keyPresses, float deltaTime);
		void Render(sf::RenderWindow& window);
		
		model::ModelParams CalculateModelParams();

		void PlaceApple();
		void WouldSnakeDieIfItMoved(cstd::Position dir);
		void WouldSnakePickUpAppleIfItMoved(cstd::Position dir);
		void PickUpApple();
		void MoveSnake(cstd::Position dir, bool grow = false);

	public:
		Game(bool useUI = true, GameControlType controlType = KEYBOARD, int gameWidth = 30, int gameHeight = 20, int windowWidth = 640, int windowHeight = 480, model::ControllerModel& controllModel = *(model::ControllerModel*)nullptr)
			: useUI(useUI), controlType(controlType), gameWidth(gameWidth), gameHeight(gameHeight), windowWidth(windowWidth), windowHeight(windowHeight), controllerModel(controllerModel)
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