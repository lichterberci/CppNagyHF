#pragma once

#include <iostream>
#include <iomanip>

#define DEFAULT_MIN_RANDOM_WEIGHT -2.0
#define DEFAULT_MAX_RANDOM_WEIGHT 2.0

namespace model {

	class ConnectionGene {

		static int s_globalInnovationNumber;
		static int GetNextInnovationNumber();

		void SetWeightToRandom(double lowerBound = DEFAULT_MIN_RANDOM_WEIGHT, double upperBound = DEFAULT_MAX_RANDOM_WEIGHT);
		void OffsetWeightByRandom(double minAmount, double maxAmount);

	public:
		int from;
		int to;
		int innovationNumber;
		bool disabled;
		double weight;

		ConnectionGene()
			: from(-1), to(-1), innovationNumber(GetNextInnovationNumber()), disabled(false), weight(0)
		{
			SetWeightToRandom();
		}

		ConnectionGene(int from, int to)
			: from(from), to(to), innovationNumber(GetNextInnovationNumber()), disabled(false), weight(0)
		{
			SetWeightToRandom();
		}

		ConnectionGene(const ConnectionGene& other)
			: from(other.from), to(other.to), innovationNumber(other.innovationNumber), disabled(other.disabled), weight(other.weight)
		{}

		ConnectionGene& operator= (const ConnectionGene& other) {
			from = other.from;
			to = other.to;
			innovationNumber = other.innovationNumber;
			disabled = other.disabled;
			weight = other.weight;

			return *this;
		}

		bool operator== (const ConnectionGene& other) const {
			return innovationNumber == other.innovationNumber;
		}

		bool operator!= (const ConnectionGene& other) const {
			return !(*this == other);
		}

		bool operator< (const ConnectionGene& other) const {
			return innovationNumber < other.innovationNumber;
		}

		bool operator> (const ConnectionGene& other) const {
			return innovationNumber > other.innovationNumber;
		}

		friend std::ostream& operator<< (std::ostream& os, const ConnectionGene& gene);

		static void SetGlobalInnovationNumber(int newInnovationNumber);	

		void MutateWeight(double changeOfMutationBeingNewRandomValue, double weightSetMin, double weightSetMax, double weightAdjustMin, double weightAdjustMax);
		void Disable();
		void Enable();
	};	

}
