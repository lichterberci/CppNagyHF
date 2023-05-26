#pragma once

#include "ActivationFunction.hpp"
#include "vector.hpp"
#include "NeatModel.hpp"
#include "FitnessFunction.hpp"
#include "SpeciesData.hpp"
#include <fstream>
#include <string>
#include "HyperparameterLoader.hpp"
#include "ModelUtils.hpp"

namespace model {

	class NeatTrainer {

	//public: // temp

		cstd::Vector<cstd::Vector<NeatModel>> organismsByGenerations;
		cstd::Vector<const NeatModel*> representativesOfThePrevGeneration;
		cstd::Vector<double> avgFitnessOfGenerations;
		cstd::Vector<SpeciesData> speciesData;
		cstd::Vector<int> speciesIndiciesOfOrganisms;

		std::unordered_map<long long, int> innovationNumberTable;

		void ConstructInitialGenerationFromFile(std::ifstream& file);
		void ConstructInitialGeneration();
		double EvaluateIndividual(const NeatModel& neatModel);
		void TrainCurrentGeneration();

		cstd::Vector<int> Speciate(const cstd::Vector<NeatModel>& organisms, cstd::Vector<SpeciesData>& speciesAgeData);
		double GetSpeciesDifferenceDelta(const NeatModel& a, const NeatModel& b);
		cstd::Vector<int> AllocatePlacesForSpecies(const cstd::Vector<double>& sumOfAdjustedFitnessForEachSpecies);

		cstd::Vector<NeatModel> ProduceNewGenerationByReproduction(
			const cstd::Vector<NeatModel>& currentGeneration, 
			const cstd::Vector<int>& speciesIndicies, 
			cstd::Vector<int>& numPlacesAllocatedForSpecies,
			const cstd::Vector<double>& rawFitnessScores
		);
		NeatModel GenerateOffSpring(const NeatModel& a, const NeatModel& b, double fitnessOfA, double fitnessOfB);

		void KeyInterruptHandler(int code) const;

	public:

		static NeatTrainer* instance;

		int populationCount = 100;
		int numGenerations = 100;
		std::shared_ptr<const ActivationFunction> activationFunction;
		std::shared_ptr<FitnessFunction> fitnessFunction;
		int numMaxIdleSteps = 10;
		unsigned int numberOfEvaluationSteps = 5;

		int gameWidth = 10;
		int gameHeight = 10;

		double chanceOfDentritInsertion = 0.3;
		double chanceOfNeuronInsertion = 0.1;
		double chanceOfDentritMutation = 0.3;
		double chanceOfMutationBeingNewValue = 0.1;
		double chanceOfDisabling = 0.1;
		double chanceOfGeneDisablingIfEitherGeneIsDisabled = 0.1;
		double weightSetMin = -2;
		double weightSetMax = 2;
		double weightAdjustMin = -0.1;
		double weightAdjustMax = 0.1;

		bool placeFirstAppleInFrontOfSnake = false;

		double portionOfSpeciesToKeepForReproduction = 0.5;
		unsigned int numGenerationsWithSameFitnessBeforeOnlyLookingAtTopSpecies = 20;
		double minImprovementOfAvgFitnessToConsiderItAnImprovement = 0.1;
		unsigned int numberOfTopSpeciesToLookAtIfFitnessIsStableForTooLong = 2;

		double neatC1 = 1;
		double neatC2 = 1;
		double neatC3 = 3;
		double neatDeltaSubT = 0.2;

		double targetFitness = std::numeric_limits<double>::max();

		int speciesDropOffAge = 15;
		double speciesDropOffFitnessThreshold = 0.05;

		NeatTrainer(const std::string& fileName) {
		
			if (instance == nullptr)
				instance = this;
			else {
				std::cerr << "ERROR: Could not create trainer object, due to instance being active!" << std::endl;
				return;
			}

			auto JSONMap = HyperparameterLoader()(fileName);

			using utils::Get;
			using utils::ToLower;

			populationCount = Get<int>(JSONMap, "PopulationCount").value_or(100);
			numGenerations = Get<int>(JSONMap, "NumGenerations").value_or(50);

			auto activationFunctionName = ToLower(Get<std::string>(JSONMap, "ActivationFunction.Name").value_or("sigmoid"));

			const std::string activationFunctionNames[] = {"sigmoid", "tanh", "relu", "lrelu"};

			bool didFindActivationFunction = false;

			for (size_t i = 0; i < sizeof(activationFunctionNames) / sizeof(std::string); i++)
				if (activationFunctionName == activationFunctionNames[i]) {
					activationFunction = utils::GenerateActivationFunctionFromTypeIndex((uint8_t)i + 1);
					didFindActivationFunction = true;
				}

			if (didFindActivationFunction == false)
				throw "Unknown activation function!";

			auto fitnessFunctionName = ToLower(Get<std::string>(JSONMap, "FitnessFunction.Name").value_or("onlyapples"));

			const std::string fitnessFunctionNames[] = { "onlyapples", "onlysteps", "applesandsteps", "applesandstepsandwin" };

			bool didFindFitnessFunciton = false;

			for (size_t i = 1; i < sizeof(fitnessFunctionNames) / sizeof(std::string); i++)
				if (fitnessFunctionName == fitnessFunctionNames[i]) {
					fitnessFunction = utils::GenerateFitnessFunctionFromTypeIndex((uint8_t)i + 1);

					if (i == 2) {
						static_cast<FitnessByApplesAndSteps*>(fitnessFunction.get())->appleCoeff = Get<double>(JSONMap, "FitnessFunction.AppleCoeff").value_or(1000);
						static_cast<FitnessByApplesAndSteps*>(fitnessFunction.get())->stepCoeff= Get<double>(JSONMap, "FitnessFunction.StepCoeff").value_or(1);
					}
					if (i == 3) {
						static_cast<FitnessByApplesAndStepsAndWin*>(fitnessFunction.get())->appleCoeff = Get<double>(JSONMap, "FitnessFunction.AppleCoeff").value_or(100);
						static_cast<FitnessByApplesAndStepsAndWin*>(fitnessFunction.get())->stepCoeff = Get<double>(JSONMap, "FitnessFunction.StepCoeff").value_or(1);
						static_cast<FitnessByApplesAndStepsAndWin*>(fitnessFunction.get())->winCoeff = Get<double>(JSONMap, "FitnessFunction.WinCoeff").value_or(1000);
					}

					didFindFitnessFunciton = true;
				}

			numMaxIdleSteps = Get<int>(JSONMap, "MaxIdleSteps").value_or(40);
			numberOfEvaluationSteps = Get<int>(JSONMap, "NumEvaluations").value_or(5);

			gameWidth = Get<int>(JSONMap, "GameWidth").value_or(3);
			gameHeight = Get<int>(JSONMap, "GameHeight").value_or(3);

			chanceOfDentritInsertion = Get<double>(JSONMap, "ChanceOfDentritInsertion").value_or(0.3);
			chanceOfNeuronInsertion = Get<double>(JSONMap, "ChanceOfNeuronInsertion").value_or(0.1);
			chanceOfDentritMutation = Get<double>(JSONMap, "ChanceOfDentritMutation").value_or(0.3);
			chanceOfMutationBeingNewValue = Get<double>(JSONMap, "ChanceOfMutationBeingNewValue").value_or(0.1);
			chanceOfDisabling = Get<double>(JSONMap, "ChanceOfDisabling").value_or(0.1);
			chanceOfGeneDisablingIfEitherGeneIsDisabled = Get<double>(JSONMap, "ChanceOfGeneDisablingIfEitherGeneIsDisabled").value_or(0.1);
			weightSetMin = Get<double>(JSONMap, "WeightSetMin").value_or(-2);
			weightSetMax = Get<double>(JSONMap, "WeightSetMax").value_or(2);
			weightAdjustMin = Get<double>(JSONMap, "WeightAdjustMin").value_or(-0.1);
			weightAdjustMax = Get<double>(JSONMap, "WeightAdjustMax").value_or(0.1);

			placeFirstAppleInFrontOfSnake = Get<bool>(JSONMap, "PlaceFirstAppleInFronOfSnake").value_or(false);

			portionOfSpeciesToKeepForReproduction = Get<double>(JSONMap, "PortionOfSpeciesToKeepForReproduction").value_or(0.5);
			numGenerationsWithSameFitnessBeforeOnlyLookingAtTopSpecies = Get<int>(JSONMap, "NumGenerationsWithSameFitnessBeforeOnlyLookingAtTopSpecies").value_or(20);
			minImprovementOfAvgFitnessToConsiderItAnImprovement = Get<double>(JSONMap, "MinImprovementOfAvgFitnessToConsiderItAnImprovement").value_or(0.1);
			numberOfTopSpeciesToLookAtIfFitnessIsStableForTooLong = Get<int>(JSONMap, "NumberOfTopSpeciesToLookAtIfFitnessIsStableForTooLong").value_or(2);

			neatC1 = Get<double>(JSONMap, "Neat.C1").value_or(1);
			neatC2 = Get<double>(JSONMap, "Neat.C2").value_or(1);
			neatC3 = Get<double>(JSONMap, "Neat.C3").value_or(3);
			neatDeltaSubT = Get<double>(JSONMap, "Neat.DeltaSubT").value_or(0.2);

			targetFitness = Get<double>(JSONMap, "TargetFitness").value_or(std::numeric_limits<double>::max());

			speciesDropOffAge = Get<double>(JSONMap, "SpeciesDropOffAge").value_or(15);
			speciesDropOffFitnessThreshold = Get<double>(JSONMap, "SpeciesDropOffFitnessThreshold").value_or(0.05);

			std::string inputProgressFileName = Get<std::string>(JSONMap, "InputProgressFile").value_or("");

			NeatModel::ResetGlobalNeuronCount();
			ConnectionGene::SetGlobalInnovationNumber(0);

			organismsByGenerations.reserve_and_copy(numGenerations); // this is essential, so the pointers can safely point to these places

			bool loadFromFile = inputProgressFileName != "";

			if (loadFromFile) {
				// check if file exists
				std::ifstream file(inputProgressFileName, std::ios::binary);

				if (file) {
					try {
						ConstructInitialGenerationFromFile(file);
					}
					catch (...) {
						loadFromFile = false;
					}

					file.close();
				}
				else {
					loadFromFile = false;
				}
			}

			if (loadFromFile == false) {
				ConstructInitialGeneration();

				NeatModel::ResetGlobalNeuronCount(NUM_SENSORS + NUM_OUTPUTS);
			}
		}

		NeatTrainer(
			int populationCount, 
			int numGenerations, 
			const std::shared_ptr<const ActivationFunction>& activationFunction,
			int maxIdleSteps,
			int gameWidth,
			int gameHeight,
			std::shared_ptr<FitnessFunction> fitnessFunction,
			const std::string& fileName = ""
		) 
			: populationCount(populationCount), 
			numGenerations(numGenerations), 
			activationFunction(activationFunction), 
			numMaxIdleSteps(maxIdleSteps),
			gameWidth(gameWidth),
			gameHeight(gameHeight),
			fitnessFunction(fitnessFunction)
		{
			if (instance == nullptr)
				instance = this;
			else {
				std::cerr << "ERROR: Could not create trainer object, due to instance being active!" << std::endl;
				return;
			}


			NeatModel::ResetGlobalNeuronCount();
			ConnectionGene::SetGlobalInnovationNumber(0);

			organismsByGenerations.reserve_and_copy(numGenerations); // this is essential, so the pointers can safely point to these places
			
			bool loadFromFile = fileName != "";

			if (loadFromFile) {
				// check if file exists
				std::ifstream file(fileName, std::ios::binary);

				if (file) {
					try {
						ConstructInitialGenerationFromFile(file);
					}
					catch (...) {
						loadFromFile = false;
					}

					file.close();
				}
				else {
					loadFromFile = false;
				}
			} 
			
			if (loadFromFile == false) {
				ConstructInitialGeneration();

				NeatModel::ResetGlobalNeuronCount(NUM_SENSORS + NUM_OUTPUTS);
			}
		}

		~NeatTrainer() {
			instance = nullptr;
		}

		inline void SetNeatConstants(double c1, double c2, double c3, double deltaSubT) {
			neatC1 = c1;
			neatC2 = c2;
			neatC3 = c3;
			neatDeltaSubT = deltaSubT;
		}

		bool Train();

		void SaveProgress(const std::string& fileName) const;

		const NeatModel* GetModelFromBestSpeciesInLastGeneration() const;
	public:
		inline cstd::Vector<SpeciesData>& GetSpecies() { return speciesData; }
	};
}
