#pragma once

#include <random>

namespace model {

	namespace utils {

		inline double RandomDouble(double min, double max) {
			return min + ((double)rand() / (RAND_MAX + 1.0)) * (max - min);
		}

		inline int RandomInt(int min, int max) {
			return min + rand() % (max - min);
		}

		inline long long MakeHashKeyFromPair(int from, int to) {
			return static_cast<long long>(from) << 32 | to;
		}

	}

}