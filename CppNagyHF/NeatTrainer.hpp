#pragma once

#include "ActivationFunction.hpp"
#include "vector.hpp"
#include "NeatModel.hpp"
#include "FitnessFunction.hpp"

namespace model {

	class NeatTrainer {

	public: // !!!TEMP!!!

		cstd::Vector<cstd::Vector<NeatModel>> organismsByGenerations;
		cstd::Vector<const NeatModel*> representativesOfThePrevGeneration;

		std::unordered_map<long long, int> innovationNumberTable;

		void ConstructInitialGeneration();
		double TrainIndividual(NeatModel& neatModel);
		void TrainGeneration();
		cstd::Vector<int> Speciate(const cstd::Vector<NeatModel>& organisms);
		double GetSpeciesDifferenceDelta(const NeatModel& a, const NeatModel& b);

	public:
		int populationCount;
		int numGenerations;
		ActivationFunction activationFunction;
		FitnessFunction fitnessFunction;
		int numMaxIdleSteps;

		int gameWidth;
		int gameHeight;

		double chanceOfDentritInsertion = 0.2;
		double chanceOfNeuronInsertion = 0.1;
		double chanceOfMutation = 0.1;
		double chanceOfMutationBeingNewValue = 0.1;
		double chanceOfDisabling = 0.1;
		double weightSetMin = 0.1;
		double weightSetMax = 0.1;
		double weightAdjustMin = 0.1;
		double weightAdjustMa = 0.1;

		double neatC1 = 1;
		double neatC2 = 1;
		double neatC3 = 3;
		double neatDeltaSubT = 0.2;

		NeatTrainer(
			int populationCount, 
			int numGenerations, 
			ActivationFunction activationFunction, 
			int maxIdleSteps,
			int gameWidth,
			int gameHeight,
			FitnessFunction fitnessFunction
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
