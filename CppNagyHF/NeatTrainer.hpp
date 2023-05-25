#pragma once

#include "ActivationFunction.hpp"
#include "vector.hpp"
#include "NeatModel.hpp"
#include "FitnessFunction.hpp"
#include "SpeciesData.hpp"
#include <fstream>
#include <string>

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

		int populationCount;
		int numGenerations;
		std::shared_ptr<const ActivationFunction> activationFunction;
		std::shared_ptr<FitnessFunction> fitnessFunction;
		int numMaxIdleSteps;
		int numBestOrganismsToKeepFromPrevGenerations = 1;
		unsigned int numberOfEvaluationSteps = 5;

		int gameWidth;
		int gameHeight;

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

		void Train();

		void SaveProgress(const std::string& fileName) const;

		const NeatModel* GetModelFromBestSpeciesInLastGeneration() const;
	public:
		inline cstd::Vector<SpeciesData>& GetSpecies() { return speciesData; }
	};
}
