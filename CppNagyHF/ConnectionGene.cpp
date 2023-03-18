#include <cstdlib>
#include "ConnectionGene.hpp"

namespace model {

	inline double RANDOM_DOUBLE(double min, double max) {
		return min + ((double)rand() / (RAND_MAX + 1.0)) * (max - min);
	}

	int ConnectionGene::s_globalInnovationNumber = 0;

	int ConnectionGene::GetNextInnovationNumber() {
		return s_globalInnovationNumber++;
	}

	void ConnectionGene::SetWeightToRandom(double lowerBound, double upperBound) {

		weight = RANDOM_DOUBLE(lowerBound, upperBound);
	}

	void ConnectionGene::OffsetWeightByRandom(double minAmount, double maxAmount) {

		weight += RANDOM_DOUBLE(minAmount, maxAmount);
	}

	void ConnectionGene::SetGlobalInnovationNumber(int newInnovationNumber) {
		s_globalInnovationNumber = newInnovationNumber;
	}

	void ConnectionGene::MutateWeight(double changeOfMutationBeingNewRandomValue, double weightSetMin, double weightSetMax, double weightAdjustMin, double weightAdjustMax) {

		if (RANDOM_DOUBLE(0, 1) < changeOfMutationBeingNewRandomValue) {
			SetWeightToRandom(weightSetMin, weightSetMax);
		}
		else {
			OffsetWeightByRandom(weightAdjustMax, weightAdjustMax);
		}
	}

	void ConnectionGene::Disable() {
		disabled = true;
	}

	void ConnectionGene::Enable() {
		disabled = false;
	}

	std::ostream& operator<< (std::ostream& os, const ConnectionGene& gene) {
		os << "Gene(from=" << gene.from << ", to=" << gene.to << ", innov=" << gene.innovationNumber << ", d=" << (gene.disabled ? "true" : "false") << ", w=" << std::setprecision(3) << gene.weight << ")";
		return os;
	}
}