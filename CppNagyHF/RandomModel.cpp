#include "RandomModel.hpp"

namespace model {

	cstd::Vector<double> RandomModel::Predict(const model::ModelParams& modelParams) const {

		/*std::cout << "Model params:" << std::endl;
		std::cout << "Distance to walls" << std::endl;
		for (const auto& d : modelParams.distancesToWall)
			std::cout << d << std::endl;
		std::cout << "Distance to apple" << std::endl;
		for (const auto& d : modelParams.distancesToApple)
			std::cout << d << std::endl;
		std::cout << "Distance to body" << std::endl;
		for (const auto& d : modelParams.distancesToBody)
			std::cout << d << std::endl;*/

		cstd::Vector<double> result;

		result += (double)(rand() % 100 - 50);
		result += (double)(rand() % 100 - 50);
		result += (double)(rand() % 100 - 50);
		result += (double)(rand() % 100 - 50);

		return result;
	}

	void RandomModel::GetKeyPresses(const model::ModelParams& modelParams, cstd::Vector<Direction>& out_keyPresses) const {
		cstd::Vector<double> result = Predict(modelParams);

		// find max value
		int choice = 0;
		double maxValue = result[0];
		for (size_t i = 1; i < std::min<int>(result.size(), 4); i++) {
			if (result[i] > maxValue) {
				maxValue = result[i];
				choice = i;
			}
		}

		Direction keys[] = {
			RIGHT, DOWN, LEFT, UP	
		};

		out_keyPresses.push(keys[choice]);
	}

}