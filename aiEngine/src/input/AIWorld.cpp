#include <input/AIWorld.h>

namespace urchin {

    AIWorld::AIWorld() {
        entities.reserve(50); //estimated memory size
    }

    /**
     * Add an entity to the AI world. Method can be called from thread different of the AI thread.
     * @param aiEntity Entity to add. Smart pointer is required to force the user to create the entity with a smart pointer. See the remove method comment for the reason to use a smart pointer.
     */
    void AIWorld::addEntity(const std::shared_ptr<AIEntity>& aiEntity) {
        std::scoped_lock lock(mutex);
        entities.push_back(aiEntity);
    }

    /**
     * Remove an entity from the AI world. Method can be called from thread different of the AI thread.
     * @param aiEntity Entity to remove. Smart pointer is used to ensure that the entity is not destroyed before it has been removed from the AI thread.
     */
    void AIWorld::removeEntity(const std::shared_ptr<AIEntity>& aiEntity) {
        std::scoped_lock lock(mutex);
        entitiesToRemove.push_back(aiEntity);
    }

    std::vector<std::shared_ptr<AIEntity>> AIWorld::getEntities() const {
        std::scoped_lock lock(mutex);
        return entities;
    }

    std::vector<std::shared_ptr<AIEntity>> AIWorld::getEntitiesToRemoveAndReset() {
        std::scoped_lock lock(mutex);
        std::vector<std::shared_ptr<AIEntity>> result = entitiesToRemove;
        for (const auto& objectToRemove : entitiesToRemove) {
            std::erase(entities, objectToRemove);
        }
        entitiesToRemove.clear();
        return result;
    }

}
