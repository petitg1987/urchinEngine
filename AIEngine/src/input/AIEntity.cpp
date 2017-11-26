#include "AIEntity.h"

namespace urchin
{
    AIEntity::AIEntity(std::string name, const Transform<float> &transform, bool bIsObstacleCandidate) :
            bToRebuild(true),
            name(std::move(name)),
            transform(transform),
            bIsObstacleCandidate(bIsObstacleCandidate)
    {

    }

    void AIEntity::updateTransform(const Point3<float> &position, const Quaternion<float> &orientation)
    {
        {
            std::lock_guard<std::mutex> lock(mutex);
            this->transform = Transform<float>(position, orientation, 1.0);
        }

        this->bToRebuild.store(true, std::memory_order_relaxed);
    }

    bool AIEntity::isToRebuild() const
    {
        return bToRebuild.load(std::memory_order_relaxed);
    }

    void AIEntity::markRebuilt()
    {
        this->bToRebuild.store(false, std::memory_order_relaxed);
    }

    const std::string &AIEntity::getName() const
    {
        return name;
    }

    Transform<float> AIEntity::getTransform() const
    {
        std::lock_guard<std::mutex> lock(mutex);
        return transform;
    }

    bool AIEntity::isObstacleCandidate() const
    {
        return bIsObstacleCandidate;
    }

}
