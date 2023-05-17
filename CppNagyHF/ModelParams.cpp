#include "ModelParams.hpp"
#include "ModelUtils.hpp"

namespace model {

	cstd::Vector<double> ModelParams::GetInputVector() const {

		cstd::Vector<double> result;
		result.reserve_and_copy(INPUT_SIZE);

#if USE_RELATIVE_DIRECTION == false

		for (const double distanceToWall : distancesToWall)
			result += distanceToWall;

		for (const double distanceToApple : distancesToApple)
			result += distanceToApple;

		for (const double distanceToBody : distancesToBody)
			result += distanceToBody;
#else

		result += blockInFront;
		result += blockToRight;
		result += blockToLeft;
		result += angleToAppleOnRight;
		result += angleToAppleOnLeft;
		result += squaresAvailableFront;
		result += squaresAvailableRight;
		result += squaresAvailableLeft;
#endif
		result += bias;

		return result;
	}

	ModelParams& ModelParams::SetToRandom() {

#if USE_RELATIVE_DIRECTION == false
		for (int i = 0; i < 8; i++)
			distancesToWall += utils::RandomDouble(0, 10);

		for (int i = 0; i < 8; i++)
			distancesToApple += utils::RandomDouble(0, 10);

		for (int i = 0; i < 8; i++)
			distancesToBody += utils::RandomDouble(0, 10);
#else
		blockInFront = utils::RandomInt(-1, 2);
		blockToRight = utils::RandomInt(-1, 2);
		blockToLeft = utils::RandomInt(-1, 2);
		angleToAppleOnRight = utils::RandomDouble(0, 1);
		angleToAppleOnLeft = utils::RandomDouble(0, 1);
		squaresAvailableFront = utils::RandomInt(0, 2);
		squaresAvailableRight = utils::RandomInt(0, 2);
		squaresAvailableLeft = utils::RandomInt(0, 2);
		currentDirection = utils::RandomInt(0, 4);
#endif
		return *this;
	}

}