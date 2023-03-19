#pragma once

#include <SFML/Graphics.hpp>
#include "vector.hpp"
#include "position.hpp"
#include "ControllerModel.hpp"
#include "RandomModel.hpp"
#include "Snake.hpp"
#include "Apple.hpp"
#include "GameReport.hpp"

#define SNAKE_SIGHT_DISTANCE 100

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
		int numIdleSteps;
		int numMaxIdleSteps;
		GameState gameState = GameState::UNINITIALIZED;
		GameControlType controlType;
		model::ControllerModel* p_controllerModel;

		Snake snake;
		Apple apple;
		unsigned int points;

		sf::Clock deltaClock;

		void RunWithUI();
		void RunWithoutUI();

		void HandleResize(sf::Event event, sf::RenderWindow& window, sf::View& view);
		bool HandleKeyPresses(sf::Event event, cstd::Vector<sf::Keyboard::Key>& out_keyPresses);
		void Update(cstd::Vector<sf::Keyboard::Key> keyPresses);
		void Render(sf::RenderWindow& window);
		
		model::ModelParams CalculateModelParams();
	public:
		Game(
			bool useUI = true, 
			GameControlType controlType = KEYBOARD, 
			int gameWidth = 30, 
			int gameHeight = 20, 
			int windowWidth = 640, 
			int windowHeight = 480, 
			model::ControllerModel& controllerModel = s_defaultModel,
			int numMaxIdleSteps = 1000
		)
			: useUI(useUI), 
			controlType(controlType), 
			gameWidth(gameWidth), 
			gameHeight(gameHeight), 
			windowWidth(windowWidth), 
			windowHeight(windowHeight), 
			p_controllerModel(&controllerModel),
			points(0),
			snakeMovesPerSec(5),
			numSteps(0),
			numIdleSteps(0),
			numMaxIdleSteps(numMaxIdleSteps)
		{}

		void Start();

		void AttachModel(model::ControllerModel& newModel) {
			p_controllerModel = &newModel;
		}

		void SetControlType(GameControlType controlType) {
			controlType = controlType;
		}

		void SetSpeed(float movesPerSec) {
			snakeMovesPerSec = movesPerSec;
		}

		void SetNumMaxIdleSteps(int numMaxIdleSteps) {
			this->numMaxIdleSteps = numMaxIdleSteps;
		}

		GameReport GenerateReport() const;
	};
}