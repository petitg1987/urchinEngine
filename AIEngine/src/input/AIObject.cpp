#include "AIObject.h"

namespace urchin
{

	AIObject::AIObject(std::shared_ptr<const ConvexShape3D<float>> shape, const Transform<float> &transform) :
			shape(shape),
			transform(transform),
			isLadder(false)
	{

	}

}
