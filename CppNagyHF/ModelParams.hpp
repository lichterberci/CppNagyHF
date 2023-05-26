#pragma once

#include "vector.hpp"
#include "position.hpp"

#define USE_RELATIVE_DIRECTION true

#define BIAS_VALUE 1.0

#if USE_RELATIVE_DIRECTION == true
#define INPUT_SIZE 9
#else
#define INPUT_SIZE 25
#endif

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

#if USE_RELATIVE_DIRECTION
		double angleToAppleOnRight = 0;
		double angleToAppleOnLeft = 0;
		double blockInFront = 0;
		double blockToRight = 0;
		double blockToLeft = 0;
		double squaresAvailableFront = 0;
		double squaresAvailableRight = 0;
		double squaresAvailableLeft = 0;
		// NOT MODEL PARAM
		// only needed for calculating relative to absolute direction
		// 0 = right, 1 = down, 2 = left, 3 = up
		int currentDirection; 
#else
		cstd::Vector<double> distancesToWall;
		cstd::Vector<double> distancesToBody;
		cstd::Vector<double> distancesToApple;
#endif
		double bias = BIAS_VALUE;

		cstd::Vector<double> GetInputVector() const;

		ModelParams& SetToRandom();
	};

}