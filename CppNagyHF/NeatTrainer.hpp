#pragma once

#include "ActivationFunction.hpp"
#include "vector.hpp"
#include "NeatModel.hpp"
#include "FitnessFunction.hpp"

namespace model {

	class NeatTrainer {

	public:

		cstd::Vector<cstd::Vector<NeatModel>> organismsByGenerations;
		cstd::Vector<const NeatModel*> representativesOfThePrevGeneration;
		cstd::Vector<double> bestFitnessesOfGenerations;

		std::unordered_map<long long, int> innovationNumberTable;

		void ConstructInitialGeneration();
		double TrainIndividual(NeatModel& neatModel);
		void TrainCurrentGeneration();

		cstd::Vector<int> Speciate(const cstd::Vector<NeatModel>& organisms);
		double GetSpeciesDifferenceDelta(const NeatModel& a, const NeatModel& b);
		cstd::Vector<int> AllocatePlacesForSpecies(const cstd::Vector<double>& sumOfAdjustedFitnessForEachSpecies);

		cstd::Vector<NeatModel> ProduceNewGenerationByReproduction(
			const cstd::Vector<NeatModel>& currentGeneration, 
			const cstd::Vector<int>& speciesIndicies, 
			const cstd::Vector<int>& numPlacesAllocatedForSpecies,
			const cstd::Vector<double>& rawFitnessScores
		);
		NeatModel GenerateOffSpring(const NeatModel& a, const NeatModel& b, double fitnessOfA, double fitnessOfB);

	public:
		int populationCount;
		int numGenerations;
		const ActivationFunction* activationFunction;
		const FitnessFunction* fitnessFunction;
		int numMaxIdleSteps;

		int gameWidth;
		int gameHeight;

		double chanceOfDentritInsertion = 0.3;
		double chanceOfNeuronInsertion = 0.1;
		double chanceOfMutation = 0.3;
		double chanceOfMutationBeingNewValue = 0.1;
		double chanceOfDisabling = 0.1;
		double weightSetMin = -2;
		double weightSetMax = 2;
		double weightAdjustMin = -0.1;
		double weightAdjustMax = 0.1;
		double portionOfSpeciesToKeepForReproduction = 0.5;
		unsigned int numGenerationsWithSameFitnessBeforeOnlyLookingAtTopSpecies = 20;
		double minVarianceInBestFitnessesToConsiderItImprovement = 3.0;
		unsigned int numberOfTopSpeciesToLookAtIfFitnessIsStableForTooLong = 2;

		double neatC1 = 1;
		double neatC2 = 1;
		double neatC3 = 3;
		double neatDeltaSubT = 0.2;

		NeatTrainer(
			int populationCount, 
			int numGenerations, 
			const ActivationFunction* activationFunction, 
			int maxIdleSteps,
			int gameWidth,
			int gameHeight,
			const FitnessFunction* fitnessFunction
		) 
			: populationCount(populationCount), 
			numGenerations(numGenerations), 
			activationFunction(activationFunction), 
			numMaxIdleSteps(maxIdleSteps),
			gameWidth(gameWidth),
			gameHeight(gameHeight),
			fitnessFunction(fitnessFunction)
		{
			NeatModel::ResetGlobalNeuronCount();
			ConnectionGene::SetGlobalInnovationNumber(0);

			ConstructInitialGeneration();

			NeatModel::ResetGlobalNeuronCount(NUM_SENSORS + NUM_OUTPUTS);
		}

		void SetNeatConstants(double c1, double c2, double c3, double deltaSubT) {
			neatC1 = c1;
			neatC2 = c2;
			neatC3 = c3;
			neatDeltaSubT = deltaSubT;
		}

		void Train();
	};
}
