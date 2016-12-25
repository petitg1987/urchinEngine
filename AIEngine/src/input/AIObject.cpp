#include "AIObject.h"

namespace urchin
{

	AIObject::AIObject(std::shared_ptr<ConvexShape3D> shape, const Transform<float> &transform) :
			shape(shape),
			transform(transform),
			isLadder(false)
	{

	}

}
