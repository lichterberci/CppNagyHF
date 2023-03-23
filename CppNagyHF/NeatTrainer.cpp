#include "NeatTrainer.hpp"
#include "Game.hpp"

namespace model {

	void NeatTrainer::ConstructInitialGeneration() {

		cstd::Vector<NeatModel> initialGeneration(populationCount);

		for (size_t i = 0; i < populationCount; i++) {
			initialGeneration[i] = NeatModel(NUM_SENSORS, NUM_OUTPUTS, activationFunction, innovationNumberTable);
		}

		organismsByGenerations += initialGeneration;
	}

	void NeatTrainer::TrainIndividual(NeatModel& neatModel) {

		auto game = game::Game(false, game::GameControlType::AI, gameWidth, gameHeight, -1, -1, neatModel, numMaxIdleSteps);

		game.Start();

		auto report = game.GenerateReport();

		double fitness = fitnessFunction(report);

	}


}
