#include "AIObject.h"

namespace urchin
{

	AIObject::AIObject(std::string name, const std::shared_ptr<AIShape> &shape, const Transform<float> &transform) :
			name(std::move(name)),
			transform(transform)
	{
		shapes.push_back(shape);
	}

	AIObject::AIObject(std::string name, const std::vector<std::shared_ptr<AIShape>> &shapes, const Transform<float> &transform) :
			name(std::move(name)),
			shapes(shapes),
			transform(transform)
	{

	}

	void AIObject::updateTransform(const Point3<float> &position, const Quaternion<float> &orientation)
	{
		std::lock_guard<std::mutex> lock(mutex);

		this->transform = Transform<float>(position, orientation, 1.0);
	}

	const std::string &AIObject::getName() const
	{
		return name;
	}

	const std::vector<std::shared_ptr<AIShape>> &AIObject::getShapes() const
	{
		return shapes;
	}

	Transform<float> AIObject::getTransform() const
	{
		std::lock_guard<std::mutex> lock(mutex);

		return transform;
	}

}
