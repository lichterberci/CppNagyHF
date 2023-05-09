#pragma once

#include "vector.hpp"
#include "ModelParams.hpp"
#include "Direction.hpp"

namespace model {

	class ControllerModel {
	public:
		virtual cstd::Vector<double> Predict(const ModelParams& modelParams) const = 0;
		virtual void GetKeyPresses(const ModelParams& modelParams, cstd::Vector<Direction>& out_keyPresses) const = 0;
	};
}