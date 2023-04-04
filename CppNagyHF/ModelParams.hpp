#pragma once

#include "vector.hpp"
#include "position.hpp"

#define BIAS_VALUE 1.0

#define INPUT_SIZE 25

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

		cstd::Vector<double> GetInputVector() const;

		ModelParams& SetToRandom();
	};

}