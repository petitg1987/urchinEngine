#include <utility>

#include "input/AIEntity.h"
#include "path/navmesh/model/NavObject.h"

namespace urchin {
    AIEntity::AIEntity(std::string name, const Transform<float>& transform, bool bIsObstacleCandidate) :
            bToRebuild(true),
            name(std::move(name)),
            transform(transform),
            bIsObstacleCandidate(bIsObstacleCandidate) {

    }

    void AIEntity::updateTransform(const Point3<float>& position, const Quaternion<float>& orientation) {
        {
            std::scoped_lock lock(mutex);
            this->transform = Transform(position, orientation);
        }

        this->bToRebuild.store(true, std::memory_order_release);
    }

    bool AIEntity::isToRebuild() const {
        return bToRebuild.load(std::memory_order_acquire);
    }

    void AIEntity::markRebuilt() {
        this->bToRebuild.store(false, std::memory_order_release);
    }

    const std::string& AIEntity::getName() const {
        return name;
    }

    Transform<float> AIEntity::getTransform() const {
        std::scoped_lock lock(mutex);
        return transform;
    }

    bool AIEntity::isObstacleCandidate() const {
        return bIsObstacleCandidate;
    }

    void AIEntity::addNavObject(const std::shared_ptr<NavObject>& navObject) {
        navObjects.push_back(navObject);
    }

    const std::vector<std::shared_ptr<NavObject>>& AIEntity::getNavObjects() const {
        return navObjects;
    }

    void AIEntity::removeAllNavObjects() {
        navObjects.clear();
    }

}
