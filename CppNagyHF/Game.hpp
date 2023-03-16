#pragma once

#include <SFML/Graphics.hpp>
#include "vector.hpp"
#include "position.hpp"
#include "ControllerModel.hpp"
#include "RandomModel.hpp"

#define SNAKE_SIGHT_DISTANCE 9999

namespace game {

	enum GameState {
		UNINITIALIZED, INITIALIZING, RUNNING, STOPPED
	};

	enum GameControlType {
		KEYBOARD, AI
	};

	class Game {
	private:	
		static model::RandomModel s_defaultModel;

		bool useUI;
		int windowWidth, windowHeight;
		int gameWidth, gameHeight;
		float snakeMovesPerSec;
		int numSteps;
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
		bool HandleKeyPresses(sf::Event event, cstd::Vector<sf::Keyboard::Key>& out_keyPresses);
		void Update(cstd::Vector<sf::Keyboard::Key> keyPresses);
		void Render(sf::RenderWindow& window);
		
		model::ModelParams CalculateModelParams();

		sf::Vector2f GetPixelOfGamePosition(const cstd::Position& pos) const;

		void InitializeSnake();
		void PlaceApple();
		bool WouldSnakeDieIfItMoved();
		bool WouldSnakePickUpAppleIfItMoved();
		void PickUpApple();
		void MoveSnake(bool grow = false);
		void UpdateHeadDirection(const sf::Keyboard::Key& key);
	public:
		Game(
			bool useUI = true, 
			GameControlType controlType = KEYBOARD, 
			int gameWidth = 30, 
			int gameHeight = 20, 
			int windowWidth = 640, 
			int windowHeight = 480, 
			model::ControllerModel& controllerModel = s_defaultModel
		)
			: useUI(useUI), 
			controlType(controlType), 
			gameWidth(gameWidth), 
			gameHeight(gameHeight), 
			windowWidth(windowWidth), 
			windowHeight(windowHeight), 
			controllerModel(controllerModel),
			points(0),
			snakeMovesPerSec(5),
			numSteps(0)
		{}

		void Start();

		void AttachModel(model::ControllerModel& newModel) {
			controllerModel = newModel;
		}

		void SetControlType(GameControlType controlType) {
			controlType = controlType;
		}

		void SetSpeed(float movesPerSec) {
			snakeMovesPerSec = movesPerSec;
		}
	};
}