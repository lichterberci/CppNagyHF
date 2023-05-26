#pragma once

#include <stdint.h>
#include <random>
#include "ActivationFunction.hpp"
#include "FitnessFunction.hpp"
#include <string>
#include "optional"
#include <unordered_map>

namespace model {

	namespace utils {

		inline double RandomDouble(double min, double max) {
			return min + ((double)rand() / (RAND_MAX + 1.0)) * (max - min);
		}

		inline int RandomInt(int min, int max) {
			return min + rand() % (max - min);
		}

		inline long long MakeHashKeyFromPair(int from, int to) {
			return static_cast<unsigned long long>((unsigned int)from) << 32 | (unsigned int)to;
		}

		inline std::shared_ptr<const ActivationFunction> GenerateActivationFunctionFromTypeIndex(uint8_t type) {
			switch (type) {
			case 1:
				return std::make_shared<Sigmoid>();
			case 2:
				return std::make_shared<Tanh>();
			case 3:
				return std::make_shared<ReLU>();
			case 4:
				return std::make_shared<LReLU>();
			default:
				return std::make_shared<Sigmoid>();
			}
		}

		inline std::shared_ptr<FitnessFunction> GenerateFitnessFunctionFromTypeIndex(uint8_t type) {
			switch (type) {
			case 1:
				return std::make_shared<FitnessByApplesOnly>();
			case 2:
				return std::make_shared<FitnessByStepsOnly>();
			case 3:
				return std::make_shared<FitnessByApplesAndSteps>();
			case 4:
				return std::make_shared<FitnessByApplesAndStepsAndWin>();
			default:
				return std::make_shared<FitnessByApplesAndSteps>();
			}
		}

		template <typename T>
		inline std::optional<T> Get(const std::unordered_map<std::string, std::string>& JSONMap, const std::string& key);

        inline std::string Trim(const std::string& str)
        {
            size_t startIndex = 0;
            size_t endIndex = str.size();

            while (std::isspace(str[startIndex]))
                startIndex++;

            while (std::isspace(str[endIndex - 1]) && endIndex > 0)
                endIndex--;

            if (endIndex == 0)
                return "";

            return str.substr(startIndex, endIndex - startIndex);
        }

        inline std::string ToLower(const std::string& str) {
            std::string result;

            for (const char c : str)
                result += (char)tolower(c);

            return result;
        }

        template <>
        inline std::optional<double> Get(const std::unordered_map<std::string, std::string>& JSONMap, const std::string& key) {
            if (JSONMap.find(key) == JSONMap.end())
                return {};

            try {
                return std::stod(Trim(JSONMap.at(key)));
            }
            catch (...) {
                return {};
            }
        }

        template <>
        inline std::optional<long long> Get(const std::unordered_map<std::string, std::string>& JSONMap, const std::string& key) {
            if (JSONMap.find(key) == JSONMap.end())
                return {};

            try {
                return std::stoll(Trim(JSONMap.at(key)));
            }
            catch (...) {
                return {};
            }
        }

        template <>
        inline std::optional<int> Get(const std::unordered_map<std::string, std::string>& JSONMap, const std::string& key) {
            if (JSONMap.find(key) == JSONMap.end())
                return {};

            try {
                return std::stoi(Trim(JSONMap.at(key)));
            }
            catch (...) {
                return {};
            }
        }

        template <>
        inline std::optional<unsigned long long> Get(const std::unordered_map<std::string, std::string>& JSONMap, const std::string& key) {
            if (JSONMap.find(key) == JSONMap.end())
                return {};

            try {
                return std::stoull(Trim(JSONMap.at(key)));
            }
            catch (...) {
                return {};
            }
        }

        template <>
        inline std::optional<std::string> Get(const std::unordered_map<std::string, std::string>& JSONMap, const std::string& key) {
            if (JSONMap.find(key) == JSONMap.end())
                return {};

            auto value = Trim(JSONMap.at(key));

            if (value == "")
                return {};

            if (value.size() < 2)
                return {};

            if (value[0] != '\"')
                return {};

            if (value.back() != '\"')
                return {};

            return value.substr(1, value.size() - 2);
        }

        template <>
        inline std::optional<bool> Get(const std::unordered_map<std::string, std::string>& JSONMap, const std::string& key) {
            if (JSONMap.find(key) == JSONMap.end())
                return {};

            auto value = Trim(JSONMap.at(key));

            if (value == "true")
                return true;
            if (value == "false")
                return false;

            return {};
        }
	}
}