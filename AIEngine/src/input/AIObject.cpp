#include "AIObject.h"

namespace urchin
{

	AIObject::AIObject(const std::string &name, std::shared_ptr<const ConvexShape3D<float>> shape, const Transform<float> &transform) :
			name(name),
			shape(std::move(shape)),
			transform(transform)
	{

	}

	const std::string &AIObject::getName() const
	{
		return name;
	}

	std::shared_ptr<const ConvexShape3D<float>> AIObject::getShape() const
	{
		return shape;
	}

	Transform<float> AIObject::getTransform() const
	{
		return transform;
	}

}
