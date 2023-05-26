#pragma once

#include <string>
#include "NeatTrainer.hpp"

namespace tests {

	inline bool ConvergenceTest(const std::string& paramsFileName, const std::string& saveProgressTo = "") {

		auto trainer = model::NeatTrainer(paramsFileName);

		// return wether target fitness is reached before termination
		bool result = trainer.Train();

		if (saveProgressTo != "")
			trainer.SaveProgress(saveProgressTo);

		return result;
	}

	inline bool ConvergenceTestFromPreTrainedModels(const std::string& paramsFileName) {

		auto trainer = model::NeatTrainer(paramsFileName);

		// return wether target fitness is reached before termination
		return trainer.Train();
	}
}