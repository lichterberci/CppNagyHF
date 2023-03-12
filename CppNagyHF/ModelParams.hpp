#pragma once

#include "vector.hpp"
#include "position.hpp"

namespace model {

/*

	Directions:

     5      6      7

        \   |   /

     4  -   H   -  0

        /   |   \

     3      2      1

*/

	struct ModelParams {

		cstd::Vector<float> distancesToWall;
		cstd::Vector<float> distancesToBody;
		cstd::Vector<float> distancesToApple;

	};

}