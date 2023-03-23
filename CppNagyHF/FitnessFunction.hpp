#pragma once

#include "GameReport.hpp"

namespace model {

	struct FitnessFunction {
		virtual double operator()(const game::GameReport& report) = 0;
	};

	struct FitnessByApplesOnly : public FitnessFunction {
		double operator()(const game::GameReport& report) override {
			return report.points;
		}
	};

	struct FitnessByStepsOnly : public FitnessFunction {
		double operator()(const game::GameReport& report) override {
			return report.numStepsTaken;
		}
	};

	template<int appleCoeff = 1000, int stepCoeff = 1>
	struct FitnessByApplesAndSteps : public FitnessFunction {
		double operator()(const game::GameReport& report) override {
			return ((double)appleCoeff * report.points + stepCoeff * report.numStepsTaken) / (appleCoeff + stepCoeff);
		}
	};
}