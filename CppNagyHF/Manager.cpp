#include "Manager.hpp"
#include <iostream>
#include "Game.hpp"
#include "NeatTrainer.hpp"
#include "Tests.hpp"

#ifndef CPORTA

void Manager::MainWithUI(int argc, char* argv[]) {

	auto args = ProcessCmdArguments(argc, argv);

	auto hasFlag = [&args](const std::string& name) {
		return args.find(name) != args.end();
	};

	auto getOrDefault = [&hasFlag, &args](const std::string& name, const std::string& defaultValue) {
		if (hasFlag(name))
			return args.at(name);

		return defaultValue;
	};

	if (hasFlag("--play")) {
		// human player

		auto game = game::Game(
			true, 
			game::GameControlType::KEYBOARD, 
			std::stoi(getOrDefault("-game-width", "6")),
			std::stoi(getOrDefault("-game-height", getOrDefault("-game-width", "6"))),
			std::stoi(getOrDefault("-window-width", "800")),
			std::stoi(getOrDefault("-window-height", getOrDefault("-window-width", "800")))
		);

		game.SetSpeed(std::stoi(getOrDefault("-speed", "5")));

		game.Start();

		return;
	}

	if (hasFlag("--train")) {

		auto trainer = model::NeatTrainer(getOrDefault("-params", ""));

		trainer.Train();

		if (hasFlag("-save"))
			trainer.SaveProgress(getOrDefault("-save", "output.progress"));

		return;
	}

	if (hasFlag("--run-tests")) {

		if (tests::ConvergenceTest(getOrDefault("-params", "params.json")) == false)
			std::cout << "TEST FAILED!" << std::endl;
		else
			std::cout << "TEST PASSED!" << std::endl;

		return;
	}

	if (hasFlag("--test-ai")) {

		auto activationFunction = model::utils::GenerateActivationFunctionFromTypeIndex(1);
		auto fitnessFunction = model::utils::GenerateFitnessFunctionFromTypeIndex(1);

		auto trainer = model::NeatTrainer(
			1,
			1,
			activationFunction,
			std::stoi(getOrDefault("-max-idle-steps", "40")),
			std::stoi(getOrDefault("-game-width", "6")),
			std::stoi(getOrDefault("-game-height", getOrDefault("-game-width", "6"))),
			fitnessFunction,
			getOrDefault("-load", "output.progress")
		);

		trainer.Train();

		while (true) {

			auto bestModel = trainer.GetModelFromBestSpeciesInLastGeneration();

			auto game = game::Game(
				true, 
				game::GameControlType::AI, 
				trainer.gameWidth, 
				trainer.gameHeight, 
				std::stoi(getOrDefault("-window-width", "800")),
				std::stoi(getOrDefault("-window-height", getOrDefault("-window-width", "800"))),
				*bestModel, 
				trainer.numMaxIdleSteps
			);

			game.SetSpeed(std::stoi(getOrDefault("-speed", "5")));

			game.Start();

			const auto report = game.GenerateReport();

			std::cout << report << std::endl;

			std::cout << "Type in 'q' to quit... ";

			if (std::cin.get() == 'q')
				break;
		}
	}
}

#endif

void Manager::MainWithoutUI(int argc, char* argv[]) {

	auto args = ProcessCmdArguments(argc, argv);

	auto hasFlag = [&args](const std::string& name) {
		return args.find(name) != args.end();
	};

	auto getOrDefault = [&hasFlag, &args](const std::string& name, const std::string& defaultValue) {
		if (hasFlag(name))
			return args.at(name);

		return defaultValue;
	};

	if (hasFlag("--train")) {

		auto trainer = model::NeatTrainer(getOrDefault("-params", ""));

		trainer.Train();

		if (hasFlag("-save"))
			trainer.SaveProgress(getOrDefault("-save", "output.progress"));

		return;
	}

	if (hasFlag("--run-tests")) {

		if (tests::ConvergenceTest(getOrDefault("-params", "params.json")) == false)
			std::cout << "TEST FAILED!" << std::endl;
		else
			std::cout << "TEST PASSED!" << std::endl;

		return;
	}
}

std::unordered_map<std::string, std::string>  Manager::ProcessCmdArguments(int argc, char* argv[])
{
	std::unordered_map<std::string, std::string> result;

	std::string key;

	for (int i = 1; i < argc; i++) {

		std::string currentItem = argv[i];

		if (currentItem.size() > 2 && currentItem[0] == '-') {

			if (currentItem[1] == '-') {
				result[currentItem] = "FLAG";
				//std::cout << currentItem << ": FLAG" << std::endl;
				key = "";
			}
			
			key = currentItem;

			continue;
		}

		if (key != "") {
			result[key] = currentItem;
			//std::cout << key << ": " << currentItem << std::endl;
		}
		else
			std::cerr << "WARNING: cmd argument is not flag: " << currentItem << std::endl;
	}

	return result;
}
