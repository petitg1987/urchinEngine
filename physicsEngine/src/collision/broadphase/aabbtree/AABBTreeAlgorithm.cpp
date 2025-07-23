#include "collision/broadphase/aabbtree/AABBTreeAlgorithm.h"
#include "shape/CollisionSphereShape.h"

namespace urchin {

    AABBTreeAlgorithm::AABBTreeAlgorithm() :
            tree(BodyAABBTree()) {

    }

    void AABBTreeAlgorithm::addBody(const std::shared_ptr<AbstractBody>& body) {
        tree.addBody(body);
    }

    void AABBTreeAlgorithm::removeBody(const AbstractBody& body) {
        tree.removeBody(body);
    }

    void AABBTreeAlgorithm::updateBodies() {
        tree.updateBodies();
    }

    const std::vector<std::unique_ptr<OverlappingPair>>& AABBTreeAlgorithm::getOverlappingPairs() const {
        return tree.getOverlappingPairs();
    }

    /**
     * @param bodiesAABBoxHitRay [out] Bodies AABBox hit by ray
     */
    void AABBTreeAlgorithm::rayTest(const Ray<float>& ray, std::vector<std::shared_ptr<AbstractBody>>& bodiesAABBoxHitRay) const {
        tree.rayQuery(ray, bodiesAABBoxHitRay);
    }

    /**
     * @param bodiesAABBoxHitBody [out] Bodies AABBox hit by a moving body
     */
    void AABBTreeAlgorithm::bodyTest(const AbstractBody& body, const PhysicsTransform& from, const PhysicsTransform& to, std::vector<std::shared_ptr<AbstractBody>>& bodiesAABBoxHitBody) const {
        Ray ray(from.getPosition(), to.getPosition());
        float bodyBoundingSphereRadius = body.getShape().getMaxDistanceToCenter();

        const auto* bodyPtr = const_cast<AbstractBody*>(&body);
        tree.enlargedRayQuery(ray, bodyBoundingSphereRadius, bodyPtr, bodiesAABBoxHitBody);
    }

}
