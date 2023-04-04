#include "ModelParams.hpp"
#include "ModelUtils.hpp"

namespace model {

	cstd::Vector<double> ModelParams::GetInputVector() const {
		cstd::Vector<double> result;
		result.reserve_and_copy(INPUT_SIZE);

		for (const double distanceToWall : distancesToWall)
			result += distanceToWall;

		for (const double distanceToApple : distancesToApple)
			result += distanceToApple;

		for (const double distanceToBody : distancesToBody)
			result += distanceToBody;

		result += bias;

		return result;
	}

	ModelParams& ModelParams::SetToRandom() {

		for (int i = 0; i < 8; i++)
			distancesToWall += utils::RandomDouble(0, 10);

		for (int i = 0; i < 8; i++)
			distancesToApple += utils::RandomDouble(0, 10);

		for (int i = 0; i < 8; i++)
			distancesToBody += utils::RandomDouble(0, 10);

		return *this;
	}

}