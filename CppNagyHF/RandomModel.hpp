#pragma once

#include "ControllerModel.hpp"

namespace model {

	class RandomModel : public ControllerModel {
	public:
		cstd::Vector<double> Predict(const model::ModelParams& modelParams) const;
		void GetKeyPresses(const ModelParams& modelParams, cstd::Vector<Direction>& out_keyPresses) const;
	};

}