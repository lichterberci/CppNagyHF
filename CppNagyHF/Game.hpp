#pragma once

#include "Direction.hpp"
#ifndef CPORTA
#include "SFML/Graphics.hpp"
#endif 
#include "vector.hpp"
#include "position.hpp"
#include "ControllerModel.hpp"
#include "RandomModel.hpp"
#include "Snake.hpp"
#include "Apple.hpp"
#include "GameReport.hpp"
#include "matrix.hpp"

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
		GameControlType controlType;
		int gameWidth, gameHeight;
		int windowWidth, windowHeight;
		const model::ControllerModel* p_controllerModel;
		unsigned int points;
		float snakeMovesPerSec;
		int numSteps;
		int numIdleSteps;
		int numMaxIdleSteps;
		bool placeFirstAppleInFrontOfSnake;
		GameState gameState = GameState::UNINITIALIZED;

		Snake snake;
		Apple apple;

#ifndef CPORTA
		sf::Clock deltaClock;
		void RunWithUI();
#endif
		void RunWithoutUI();

#ifndef CPORTA
		void HandleResize(sf::Event event, sf::RenderWindow& window, sf::View& view);
		bool HandleKeyPresses(sf::Event event, cstd::Vector<model::Direction>& out_keyPresses);
#endif
		void Update(cstd::Vector<model::Direction> keyPresses);
#ifndef CPORTA
		void Render(sf::RenderWindow& window);
#endif
		model::ModelParams CalculateModelParams() const;
		int CalculateAvailableSquares(const cstd::Position& pos, cstd::Matrix<bool>& checkedSquares, const cstd::Matrix<bool>& walkableSquares) const;
	public:
		Game(
			bool useUI = true, 
			GameControlType controlType = KEYBOARD, 
			int gameWidth = 30, 
			int gameHeight = 20, 
			int windowWidth = 640, 
			int windowHeight = 480, 
			const model::ControllerModel& controllerModel = s_defaultModel,
			int numMaxIdleSteps = 1000,
			bool placeFirstAppleInFrontOfSnake = true
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
			numMaxIdleSteps(numMaxIdleSteps),
			placeFirstAppleInFrontOfSnake(placeFirstAppleInFrontOfSnake)
		{}

		void Start();

		void AttachModel(model::ControllerModel& newModel) {
			p_controllerModel = &newModel;
		}

		void SetControlType(GameControlType _controlType) {
			controlType = _controlType;
		}

		void SetSpeed(float movesPerSec) {
			snakeMovesPerSec = movesPerSec;
		}

		void SetNumMaxIdleSteps(int maxIdleSteps) {
			this->numMaxIdleSteps = maxIdleSteps;
		}

		GameReport GenerateReport() const;
	};
}