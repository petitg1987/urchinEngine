#include <collision/broadphase/aabbtree/AABBTreeAlgorithm.h>
#include <shape/CollisionSphereShape.h>

namespace urchin {

    AABBTreeAlgorithm::AABBTreeAlgorithm() :
            tree(std::make_unique<BodyAABBTree>()) {

    }

    void AABBTreeAlgorithm::addBody(AbstractBody& body) {
        tree->addBody(body);
    }

    void AABBTreeAlgorithm::removeBody(const AbstractBody& body) {
        tree->removeBody(body);
    }

    void AABBTreeAlgorithm::updateBodies() {
        tree->updateBodies();
    }

    const std::vector<std::unique_ptr<OverlappingPair>>& AABBTreeAlgorithm::getOverlappingPairs() const {
        return tree->getOverlappingPairs();
    }

    std::vector<AbstractBody*> AABBTreeAlgorithm::rayTest(const Ray<float>& ray) const {
        std::vector<AbstractBody*> bodiesAABBoxHitRay;
        bodiesAABBoxHitRay.reserve(10);

        tree->rayQuery(ray, bodiesAABBoxHitRay);

        return bodiesAABBoxHitRay;
    }

    std::vector<AbstractBody*> AABBTreeAlgorithm::bodyTest(const AbstractBody& body, const PhysicsTransform& from, const PhysicsTransform& to) const {
        std::vector<AbstractBody*> bodiesAABBoxHitBody;
        bodiesAABBoxHitBody.reserve(15);

        Ray<float> ray(from.getPosition(), to.getPosition());
        float bodyBoundingSphereRadius = body.getShape().getMaxDistanceToCenter();

        auto* bodyPtr = const_cast<AbstractBody*>(&body);
        tree->enlargedRayQuery(ray, bodyBoundingSphereRadius, bodyPtr, bodiesAABBoxHitBody);

        return bodiesAABBoxHitBody;
    }

}
