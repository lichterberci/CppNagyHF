#include <cstdlib>
#include "ConnectionGene.hpp"
#include "ModelUtils.hpp"
#include <iomanip>

namespace model {

	int ConnectionGene::s_globalInnovationNumber = 0;

	int ConnectionGene::GetNextInnovationNumber() {
		return s_globalInnovationNumber++;
	}

	void ConnectionGene::SetWeightToRandom(double lowerBound, double upperBound) {

		weight = utils::RandomDouble(lowerBound, upperBound);
	}

	void ConnectionGene::OffsetWeightByRandom(double minAmount, double maxAmount) {

		weight += utils::RandomDouble(minAmount, maxAmount);
	}

	void ConnectionGene::SetGlobalInnovationNumber(int newInnovationNumber) {
		s_globalInnovationNumber = newInnovationNumber;
	}

	void ConnectionGene::MutateWeight(double changeOfMutationBeingNewRandomValue, double weightSetMin, double weightSetMax, double weightAdjustMin, double weightAdjustMax) {

		if (utils::RandomDouble(0, 1) < changeOfMutationBeingNewRandomValue) {
			SetWeightToRandom(weightSetMin, weightSetMax);
		}
		else {
			OffsetWeightByRandom(weightAdjustMin, weightAdjustMax);
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

	void ConnectionGene::SetInnovationNumber(std::unordered_map<long long, int>& innovationNumberTable) {

		if (innovationNumberTable.find(utils::MakeHashKeyFromPair(from, to)) != innovationNumberTable.end()) {

			innovationNumber = innovationNumberTable[utils::MakeHashKeyFromPair(from, to)];

			return;
		}

		innovationNumber = GetNextInnovationNumber();

		//std::cout << "Set innovation number: from=" << from << ", to=" << to << ", hash=" << utils::MakeHashKeyFromPair(from, to) << " --> " << innovationNumber << std::endl;

		innovationNumberTable[utils::MakeHashKeyFromPair(from, to)] = innovationNumber;
	}
}