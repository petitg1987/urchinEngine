#include <collision/broadphase/BroadPhaseManager.h>
#include <collision/broadphase/aabbtree/AABBTreeAlgorithm.h>

namespace urchin {

    BroadPhaseManager::BroadPhaseManager(BodyManager& bodyManager) :
            broadPhaseAlgorithm(std::make_unique<AABBTreeAlgorithm>()) {

        bodyManager.addObserver(this, BodyManager::ADD_BODY);
        bodyManager.addObserver(this, BodyManager::REMOVE_BODY);
    }

    BroadPhaseManager::~BroadPhaseManager() {
        synchronizeBodies();
    }

    void BroadPhaseManager::notify(Observable* observable, int notificationType) {
        if (auto* bodyManager = dynamic_cast<BodyManager*>(observable)) {
            if (notificationType == BodyManager::ADD_BODY) {
                addBody(*bodyManager->getLastUpdatedBody());
            } else if (notificationType == BodyManager::REMOVE_BODY) {
                removeBody(*bodyManager->getLastUpdatedBody());
            }
        }
    }

    /**
     * Add a body to the broad phase. Method can be called from thread different of the physics thread.
     * @param body Body to add. Smart pointer is required to force the user to create the body with a smart pointer. See the remove method comment for the reason to use a smart pointer.
     */
    void BroadPhaseManager::addBodyAsync(const std::shared_ptr<AbstractBody>& body) {
        std::lock_guard<std::mutex> lock(mutex);
        bodiesToAdd.push_back(body.get());
    }

    void BroadPhaseManager::addBody(AbstractBody& body) {
        broadPhaseAlgorithm->addBody(body);
    }

    /**
     * Remove a body from the broad phase. Method can be called from thread different of the physics thread.
     * @param body Body to remove. Smart pointer is used to ensure that the body is not destroyed before it has been removed from the physics thread.
     */
    void BroadPhaseManager::removeBodyAsync(const std::shared_ptr<AbstractBody>& body) {
        std::lock_guard<std::mutex> lock(mutex);
        bodiesToRemove.push_back(body);
    }

    void BroadPhaseManager::removeBody(const AbstractBody& body) {
        broadPhaseAlgorithm->removeBody(body);
    }

    void BroadPhaseManager::synchronizeBodies() {
        std::lock_guard<std::mutex> lock(mutex);

        for (auto& bodyToAdd : bodiesToAdd) {
            addBody(*bodyToAdd);
        }
        bodiesToAdd.clear();

        for (auto& bodyToRemove : bodiesToRemove) {
            removeBody(*bodyToRemove);
        }
        bodiesToRemove.clear();
    }

    const std::vector<std::unique_ptr<OverlappingPair>>& BroadPhaseManager::computeOverlappingPairs() {
        ScopeProfiler sp(Profiler::physics(), "coOverlapPair");

        synchronizeBodies();

        broadPhaseAlgorithm->updateBodies();
        return broadPhaseAlgorithm->getOverlappingPairs();
    }

    std::vector<AbstractBody*> BroadPhaseManager::rayTest(const Ray<float>& ray) const {
        return broadPhaseAlgorithm->rayTest(ray);
    }

    std::vector<AbstractBody*> BroadPhaseManager::bodyTest(const AbstractBody& body, const PhysicsTransform& from, const PhysicsTransform& to) const {
        return broadPhaseAlgorithm->bodyTest(body, from, to);
    }

}
