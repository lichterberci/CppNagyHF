#pragma once

#include <string>
#include "NeatTrainer.hpp"

namespace tests {

	bool ConvergenceTest(const std::string& paramsFileName) {

		auto trainer = model::NeatTrainer(paramsFileName);

		// return wether target fitness is reached before termination
		return trainer.Train();
	}
}