#pragma once

#include <string>
#include "NeatTrainer.hpp"
#include <optional>

namespace model {

	class HyperparameterLoader {

	public:
		NeatTrainer operator()(const std::string& fileName) const;
	private:
		static std::string Trim(const std::string& str);
		template <typename T>
		static std::optional<T> Get(const std::unordered_map<std::string, std::string>& JSONMap, const std::string& key);
	};

}