#include <collision/broadphase/BroadPhaseManager.h>
#include <collision/broadphase/aabbtree/AABBTreeAlgorithm.h>

namespace urchin {

    BroadPhaseManager::BroadPhaseManager(BodyManager& bodyManager) :
            broadPhaseAlgorithm(new AABBTreeAlgorithm()) {

        bodyManager.addObserver(this, BodyManager::ADD_BODY);
        bodyManager.addObserver(this, BodyManager::REMOVE_BODY);
    }

    BroadPhaseManager::~BroadPhaseManager() {
        synchronizeBodies();
        delete broadPhaseAlgorithm;
    }

    void BroadPhaseManager::notify(Observable* observable, int notificationType) {
        if (auto* bodyManager = dynamic_cast<BodyManager*>(observable)) {
            if (notificationType == BodyManager::ADD_BODY) {
                addBody(bodyManager->getLastUpdatedBody());
            } else if (notificationType == BodyManager::REMOVE_BODY) {
                removeBody(bodyManager->getLastUpdatedBody());
            }
        }
    }

    /**
     * Add body to broad phase. Method can be called from thread different of the physics thread.
     */
    void BroadPhaseManager::addBodyAsync(AbstractBody* body) {
        std::lock_guard<std::mutex> lock(mutex);

        bodiesToAdd.push_back(body);
    }

    void BroadPhaseManager::addBody(AbstractBody* body) {
        broadPhaseAlgorithm->addBody(body);
    }

    /**
     * Remove body from broad phase. Method can be called from thread different of the physics thread.
     */
    void BroadPhaseManager::removeBodyAsync(AbstractBody* body) {
        std::lock_guard<std::mutex> lock(mutex);

        bodiesToRemove.push_back(body);
    }

    void BroadPhaseManager::removeBody(AbstractBody* body) {
        broadPhaseAlgorithm->removeBody(body);
    }

    void BroadPhaseManager::synchronizeBodies() {
        std::lock_guard<std::mutex> lock(mutex);

        for (auto& bodyToAdd : bodiesToAdd) {
            addBody(bodyToAdd);
        }
        bodiesToAdd.clear();

        for (auto& bodyToRemove : bodiesToRemove) {
            removeBody(bodyToRemove);
            delete bodyToRemove;
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

    std::vector<AbstractBody*> BroadPhaseManager::bodyTest(AbstractBody* body, const PhysicsTransform& from, const PhysicsTransform& to) const {
        return broadPhaseAlgorithm->bodyTest(body, from, to);
    }

}
