#include "AIObject.h"

namespace urchin
{

	AIObject::AIObject(std::string name, const std::shared_ptr<AIShape> &shape, const Transform<float> &transform, bool bIsObstacleCandidate) :
            bToRebuild(true),
			name(std::move(name)),
			bIsDissociateShapes(true),
			transform(transform),
			bIsObstacleCandidate(bIsObstacleCandidate)
	{
		shapes.push_back(shape);
	}

	AIObject::AIObject(std::string name, bool isDissociateShapes, const std::vector<std::shared_ptr<AIShape>> &shapes, const Transform<float> &transform,
					   bool bIsObstacleCandidate) :
			bToRebuild(true),
			name(std::move(name)),
			bIsDissociateShapes(isDissociateShapes),
			shapes(shapes),
			transform(transform),
			bIsObstacleCandidate(bIsObstacleCandidate)
	{

	}

	void AIObject::updateTransform(const Point3<float> &position, const Quaternion<float> &orientation)
	{
		{
			std::lock_guard<std::mutex> lock(mutex);
			this->transform = Transform<float>(position, orientation, 1.0);
		}

        this->bToRebuild.store(true, std::memory_order_relaxed);
	}

	bool AIObject::isToRebuild() const
	{
		return bToRebuild.load(std::memory_order_relaxed);
	}

	void AIObject::markRebuilt()
	{
		this->bToRebuild.store(false, std::memory_order_relaxed);
	}

	const std::string &AIObject::getName() const
	{
		return name;
	}

	bool AIObject::isDissociateShapes() const
	{
		return bIsDissociateShapes;
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

	bool AIObject::isObstacleCandidate() const
	{
		return bIsObstacleCandidate;
	}

}
