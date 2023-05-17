#pragma once

#include "GameReport.hpp"
#include "ModelUtils.hpp"
#include <math.h>

namespace model {

	struct FitnessFunction {
		virtual double operator()(const game::GameReport& report) const = 0;
	};

	struct FitnessByApplesOnly : public FitnessFunction {
		double operator()(const game::GameReport& report) const override {
			return report.points;
		}
	};

	struct FitnessByStepsOnly : public FitnessFunction {
		double operator()(const game::GameReport& report) const override {
			return report.numStepsTaken;
		}
	};

	template<int appleCoeff = 1000, int stepCoeff = 1>
	struct FitnessByApplesAndSteps : public FitnessFunction {
		double operator()(const game::GameReport& report) const override {
			return std::max((
				(double)appleCoeff * report.points 
				+ stepCoeff * report.numStepsTaken
			) / (abs(appleCoeff) + abs(stepCoeff)), 0.0);
		}
	};

	template<int appleCoeff = 1000, int stepCoeff = 1, int winCoeff = 1000000>
	struct FitnessByApplesAndStepsAndWin : public FitnessFunction {
		double operator()(const game::GameReport& report) const override {
			return std::max((
				(double)appleCoeff * report.points 
				+ stepCoeff * report.numStepsTaken 
				+ winCoeff * (report.won == true)
			) / (abs(appleCoeff) + abs(stepCoeff) + abs(winCoeff)), 0.0);
		}
	};
}