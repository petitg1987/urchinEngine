#include "collision/broadphase/BroadPhase.h"
#include "collision/broadphase/aabbtree/AABBTreeAlgorithm.h"

namespace urchin {

    BroadPhase::BroadPhase(BodyContainer& bodyContainer) :
            broadPhaseAlgorithm(AABBTreeAlgorithm()) {
        bodyContainer.addObserver(this, BodyContainer::ADD_BODY);
        bodyContainer.addObserver(this, BodyContainer::REMOVE_BODY);
    }

    BroadPhase::~BroadPhase() {
        synchronizeBodies();
    }

    void BroadPhase::notify(Observable* observable, int notificationType) {
        if (const auto* bodyContainer = dynamic_cast<BodyContainer*>(observable)) {
            if (notificationType == BodyContainer::ADD_BODY) {
                addBody(bodyContainer->getLastUpdatedBody());
            } else if (notificationType == BodyContainer::REMOVE_BODY) {
                removeBody(*bodyContainer->getLastUpdatedBody());
            }
        }
    }

    /**
     * Add a body to the broad phase. Method can be called from thread different of the physics thread.
     * @param body Body to add. Smart pointer is required to force the user to create the body with a smart pointer. See the remove method comment for the reason to use a smart pointer.
     */
    void BroadPhase::addBodyAsync(const std::shared_ptr<AbstractBody>& body) {
        std::scoped_lock lock(mutex);
        bodiesToAdd.push_back(body);
    }

    void BroadPhase::addBody(const std::shared_ptr<AbstractBody>& body) {
        broadPhaseAlgorithm.addBody(body);
    }

    /**
     * Remove a body from the broad phase. Method can be called from thread different of the physics thread.
     * @param body Body to remove. Smart pointer is used to ensure that the body is not destroyed before it has been removed from the physics thread.
     */
    void BroadPhase::removeBodyAsync(const std::shared_ptr<AbstractBody>& body) {
        std::scoped_lock lock(mutex);
        bodiesToRemove.push_back(body);
    }

    void BroadPhase::removeBody(const AbstractBody& body) {
        broadPhaseAlgorithm.removeBody(body);
    }

    void BroadPhase::synchronizeBodies() {
        std::scoped_lock lock(mutex);

        for (const auto& bodyToAdd : bodiesToAdd) {
            addBody(bodyToAdd);
        }
        bodiesToAdd.clear();

        for (const auto& bodyToRemove : bodiesToRemove) {
            removeBody(*bodyToRemove);
        }
        bodiesToRemove.clear();
    }

    const std::vector<std::unique_ptr<OverlappingPair>>& BroadPhase::computeOverlappingPairs() {
        ScopeProfiler sp(Profiler::physics(), "coOverlapPair");

        synchronizeBodies();

        broadPhaseAlgorithm.updateBodies();
        return broadPhaseAlgorithm.getOverlappingPairs();
    }

    /**
     * @param bodiesAABBoxHitRay [out] Bodies AABBox hit by ray
     */
    void BroadPhase::rayTest(const Ray<float>& ray, std::vector<std::shared_ptr<AbstractBody>>& bodiesAABBoxHitRay) const {
        assert(bodiesAABBoxHitRay.empty());
        broadPhaseAlgorithm.rayTest(ray, bodiesAABBoxHitRay);
    }

    /**
     * @param bodiesAABBoxHitBody [out] Bodies AABBox hit by a moving body
     */
    void BroadPhase::bodyTest(const AbstractBody& body, const PhysicsTransform& from, const PhysicsTransform& to, std::vector<std::shared_ptr<AbstractBody>>& bodiesAABBoxHitBody) const {
        assert(bodiesAABBoxHitBody.empty());
        broadPhaseAlgorithm.bodyTest(body, from, to, bodiesAABBoxHitBody);
    }

}
