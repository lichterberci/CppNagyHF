#pragma once

#include <iostream>
#include <iomanip>
#include <unordered_map>

#define DEFAULT_MIN_RANDOM_WEIGHT -2.0
#define DEFAULT_MAX_RANDOM_WEIGHT 2.0


namespace model {

	class ConnectionGene {

		static int s_globalInnovationNumber;
		static int GetNextInnovationNumber();

		void SetWeightToRandom(double lowerBound = DEFAULT_MIN_RANDOM_WEIGHT, double upperBound = DEFAULT_MAX_RANDOM_WEIGHT);
		void OffsetWeightByRandom(double minAmount, double maxAmount);
		void SetInnovationNumber(std::unordered_map<long long, int>& innovationNumberTable);

	public:
		int from;
		int to;
		int innovationNumber;
		bool disabled;
		double weight;

		ConnectionGene()
			: from(-1), to(-1), innovationNumber(-1), disabled(true), weight(0)
		{
			SetWeightToRandom();
		}

		ConnectionGene(int from, int to, std::unordered_map<long long, int>& innovationNumberTable)
			: from(from), to(to), disabled(false), weight(0)
		{
			SetWeightToRandom();
			SetInnovationNumber(innovationNumberTable);
		}

		ConnectionGene(int from, int to, int innovationNumber)
			: from(from), to(to), innovationNumber(innovationNumber), disabled(false), weight(0)
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

		bool operator<= (const ConnectionGene& other) const {
			return innovationNumber <= other.innovationNumber;
		}

		bool operator>= (const ConnectionGene& other) const {
			return innovationNumber >= other.innovationNumber;
		}

		friend std::ostream& operator<< (std::ostream& os, const ConnectionGene& gene);

		static void SetGlobalInnovationNumber(int newInnovationNumber);	

		void MutateWeight(double changeOfMutationBeingNewRandomValue, double weightSetMin, double weightSetMax, double weightAdjustMin, double weightAdjustMax);
		void Disable();
		void Enable();

		friend std::ostream& operator<< (std::ostream& os, const ConnectionGene& gene);
	};	

}
