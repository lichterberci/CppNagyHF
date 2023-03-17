#pragma once

#include "vector.hpp"
#include "position.hpp"

#define BIAS_VALUE 1.0

#define INPUT_SIZE

namespace model {

/*

	Directions:

     5      6      7

        \   |   /

     4  -       -  0

        /   |   \

     3      2      1

*/

	struct ModelParams {

		cstd::Vector<double> distancesToWall;
		cstd::Vector<double> distancesToBody;
		cstd::Vector<double> distancesToApple;
		double bias = BIAS_VALUE;

		cstd::Vector<double> GetInputVector() const {
			auto result = distancesToWall;

			for (const double distanceToApple : distancesToApple)
				result += distanceToApple;

			for (const double distanceToBody : distancesToBody)
				result += distanceToBody;

			result += bias;

			return result;
		}

	};

}