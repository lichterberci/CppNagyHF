#pragma once

#include "ControllerModel.hpp"

namespace model {

	class RandomModel : public ControllerModel {
	public:
		cstd::Vector<double> Predict(const model::ModelParams& modelParams);
		void GetKeyPresses(const ModelParams& modelParams, cstd::Vector<sf::Keyboard::Key>& out_keyPresses);
	};

}