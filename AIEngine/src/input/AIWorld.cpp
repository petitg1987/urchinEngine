#include <algorithm>

#include <input/AIWorld.h>

namespace urchin {

    AIWorld::AIWorld() {
        entities.reserve(50); //estimated memory size
    }

    /**
     * Add a body to the AI world. Method can be called from thread different of the AI thread.
     * @param body Body to add. Smart pointer is required to force the user to create the body with a smart pointer. See the remove method comment for the reason to use a smart pointer.
     */
    void AIWorld::addEntity(const std::shared_ptr<AIEntity>& aiEntity) {
        std::lock_guard<std::mutex> lock(mutex);
        entities.push_back(aiEntity);
    }

    /**
     * Remove a body from the AI world. Method can be called from thread different of the AI thread.
     * @param body Body to remove. Smart pointer is used to ensure that the body is not destroyed before it has been removed from the AI thread.
     */
    void AIWorld::removeEntity(const std::shared_ptr<AIEntity>& aiEntity) {
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
        for (const auto& objectToRemove : entitiesToRemove) {
            entities.erase(std::remove(entities.begin(), entities.end(), objectToRemove), entities.end());
        }
        entitiesToRemove.clear();
        return result;
    }

}
