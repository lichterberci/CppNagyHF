#pragma once

#include "vector.hpp"
#include "ModelParams.hpp"
#include <SFML/Graphics.hpp>

namespace model {

	class ControllerModel {
	public:
		cstd::Vector<double> Predict(const ModelParams& modelParams) {}
		void GetKeyPresses(const ModelParams& modelParams, cstd::Vector<sf::Keyboard::Key> keyPresses) {}
	};

}