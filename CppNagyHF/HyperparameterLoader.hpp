#pragma once

#include <unordered_map>
#include <string>

namespace model {

	class HyperparameterLoader {
	public:
		std::unordered_map<std::string, std::string> operator()(const std::string& fileName) const;
	};
}