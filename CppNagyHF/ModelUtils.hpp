#pragma once

#include <stdint.h>
#include <random>
#include "ActivationFunction.hpp"

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
	}
}