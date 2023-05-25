#pragma once

#include "GameReport.hpp"
#include <math.h>
#include <iostream>

namespace model {

	struct FitnessFunction {
		virtual double operator()(const game::GameReport& report) const = 0;
		virtual uint8_t GetTypeIndex() const = 0;
		virtual void WriteCoeffs(std::ostream& os) const {}
		virtual void ReadAndSetUpCoeffs(std::istream& is) {}
	};

	struct FitnessByApplesOnly : public FitnessFunction {
		double operator()(const game::GameReport& report) const override {
			return report.points;
		}

		uint8_t GetTypeIndex() const override {
			return 1;
		}
	};

	struct FitnessByStepsOnly : public FitnessFunction {
		double operator()(const game::GameReport& report) const override {
			return report.numStepsTaken;
		}

		uint8_t GetTypeIndex() const override {
			return 2;
		}
	};

	struct FitnessByApplesAndSteps : public FitnessFunction {
		double appleCoeff, stepCoeff;

		FitnessByApplesAndSteps(double _appleCoeff = 1000, double _stepCoeff = 1)
			: appleCoeff(_appleCoeff), stepCoeff(_stepCoeff)
		{}

		double operator()(const game::GameReport& report) const override {
			return std::max((
				(double)appleCoeff * report.points 
				+ stepCoeff * report.numStepsTaken
			) / (abs(appleCoeff) + abs(stepCoeff)), 0.0);
		}

		uint8_t GetTypeIndex() const override {
			return 3;
		}

		void WriteCoeffs(std::ostream& os) const override {
			os.write((char*)&appleCoeff, sizeof(double));
			os.write((char*)&stepCoeff, sizeof(double));
		}

		void ReadAndSetUpCoeffs(std::istream& is) override {
			is.read((char*)&appleCoeff, sizeof(double));
			is.read((char*)&stepCoeff, sizeof(double));
		}
	};

	struct FitnessByApplesAndStepsAndWin : public FitnessFunction {
		double appleCoeff, stepCoeff, winCoeff;

		FitnessByApplesAndStepsAndWin(double _appleCoeff = 100, double _stepCoeff = 1, double _winCoeff = 1000)
			: appleCoeff(_appleCoeff), stepCoeff(_stepCoeff), winCoeff(_winCoeff)
		{}

		double operator()(const game::GameReport& report) const override {
			return std::max((
				(double)appleCoeff * report.points 
				+ stepCoeff * report.numStepsTaken 
				+ winCoeff * (report.won == true)
			) / (abs(appleCoeff) + abs(stepCoeff) + abs(winCoeff)), 0.0);
		}

		uint8_t GetTypeIndex() const override {
			return 4;
		}

		void WriteCoeffs(std::ostream& os) const override {
			os.write((char*)&appleCoeff, sizeof(double));
			os.write((char*)&stepCoeff, sizeof(double));
			os.write((char*)&winCoeff, sizeof(double));
		}

		void ReadAndSetUpCoeffs(std::istream& is) override {
			is.read((char*) & appleCoeff, sizeof(double));
			is.read((char*) & stepCoeff, sizeof(double));
			is.read((char*) & winCoeff, sizeof(double));
		}
	};
}