#pragma once

#include <string>
#include "vector.hpp"
#include "NeatModel.hpp"

namespace model {

	class Persistence {
	public:
		static void SaveModelsToFile(const std::string& fileName, const cstd::Vector<NeatModel>& models);
		static cstd::Vector<NeatModel> LoadModelsFromFile(const std::string& fileName);
	};

}