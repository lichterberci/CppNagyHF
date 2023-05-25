#include "Persistence.hpp"

#include <iostream>
#include <fstream>

namespace model {

	void model::Persistence::SaveModelsToFile(const std::string& fileName, const cstd::Vector<NeatModel>& models)
	{
		std::ofstream file(fileName, std::ios::binary);

		for (const auto& model : models)
			model.Serialize(file);

		file.close();
	}

	cstd::Vector<NeatModel> model::Persistence::LoadModelsFromFile(const std::string& fileName)
	{
		cstd::Vector<NeatModel> result;

		std::ifstream file(fileName, std::ios::binary);

		while (file.eof() == false) {
			NeatModel model;
			model.DesrializeAndSetUp(file);
			result += model;
		}

		return result;
	}

}