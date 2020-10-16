#include <algorithm>

#include "AIWorld.h"

namespace urchin {

    AIWorld::AIWorld() {
        entities.reserve(50); //estimated memory size
    }

    void AIWorld::addEntity(const std::shared_ptr<AIEntity> &aiEntity) {
        std::lock_guard<std::mutex> lock(mutex);
        entities.push_back(aiEntity);
    }

    void AIWorld::removeEntity(const std::shared_ptr<AIEntity> &aiEntity) {
        std::lock_guard<std::mutex> lock(mutex);
        entitiesToRemove.push_back(aiEntity);
    }

    std::vector<std::shared_ptr<AIEntity>> AIWorld::getEntities() const {
        std::lock_guard<std::mutex> lock(mutex);
        return entities;
    }

    std::vector<std::shared_ptr<AIEntity>> AIWorld::getEntitiesToRemoveAndReset() {
        std::lock_guard<std::mutex> lock(mutex);
        std::vector<std::shared_ptr<AIEntity>> result = entitiesToRemove;
        for(const auto &objectToRemove : entitiesToRemove) {
            entities.erase(std::remove(entities.begin(), entities.end(), objectToRemove), entities.end());
        }
        entitiesToRemove.clear();
        return result;
    }

}
