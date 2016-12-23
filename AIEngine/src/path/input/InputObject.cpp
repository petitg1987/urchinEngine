#include "InputObject.h"

namespace urchin
{

	InputObject::InputObject(std::shared_ptr<ConvexObject3D<float>> object) :
			object(object),
			isLadder(false)
	{

	}

}
