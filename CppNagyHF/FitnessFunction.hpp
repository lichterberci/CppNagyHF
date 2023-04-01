#pragma once

#include "GameReport.hpp"
#include "ModelUtils.hpp"

namespace model {

	struct FitnessFunction {
		virtual double operator()(const game::GameReport& report) const {
			return utils::RandomDouble(0, 100);
		}
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
			return (
				(double)appleCoeff * report.points 
				+ stepCoeff * report.numStepsTaken
			) / (appleCoeff + stepCoeff);
		}
	};

	template<int appleCoeff = 1000, int stepCoeff = 1, int winCoeff = 1000000>
	struct FitnessByApplesAndStepsAndWin : public FitnessFunction {
		double operator()(const game::GameReport& report) const override {
			return (
				(double)appleCoeff * report.points 
				+ stepCoeff * report.numStepsTaken 
				+ winCoeff * (report.won == true)
			) / (appleCoeff + stepCoeff + winCoeff);
		}
	};
}