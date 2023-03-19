#pragma once

#include "ActivationFunction.hpp"
#include "vector.hpp"

namespace model {

	class NeatTrainer {
	private:

		int populationCount;
		int numGenerations;
		ActivationFunction activationFunction;
		int numMaxIdleSteps;

		double mutationChance = 0.3;
		double chanceOfMutationBeingNewValue = 0.1;

		double neatC1 = 1;
		double neatC2 = 1;
		double neatC3 = 3;
		double neatDeltaSubT = 0.2;

	public:


	};

}
