#include <collision/broadphase/aabbtree/AABBTreeAlgorithm.h>
#include <shape/CollisionSphereShape.h>

namespace urchin {

    AABBTreeAlgorithm::AABBTreeAlgorithm() :
            tree(new BodyAABBTree()) {

    }

    AABBTreeAlgorithm::~AABBTreeAlgorithm() {
        delete tree;
    }

    void AABBTreeAlgorithm::addBody(AbstractBody* body) {
        tree->addBody(body);
    }

    void AABBTreeAlgorithm::removeBody(AbstractBody* body) {
        tree->removeBody(body);
    }

    void AABBTreeAlgorithm::updateBodies() {
        tree->updateBodies();
    }

    const std::vector<OverlappingPair*>& AABBTreeAlgorithm::getOverlappingPairs() const {
        return tree->getOverlappingPairs();
    }

    std::vector<AbstractBody*> AABBTreeAlgorithm::rayTest(const Ray<float>& ray) const {
        std::vector<AbstractBody*> bodiesAABBoxHitRay;
        bodiesAABBoxHitRay.reserve(10);

        tree->rayQuery(ray, bodiesAABBoxHitRay);

        return bodiesAABBoxHitRay;
    }

    std::vector<AbstractBody*> AABBTreeAlgorithm::bodyTest(AbstractBody* body, const PhysicsTransform& from, const PhysicsTransform& to) const {
        std::vector<AbstractBody*> bodiesAABBoxHitBody;
        bodiesAABBoxHitBody.reserve(15);

        Ray<float> ray(from.getPosition(), to.getPosition());
        float bodyBoundingSphereRadius = body->getShape().getMaxDistanceToCenter();

        tree->enlargedRayQuery(ray, bodyBoundingSphereRadius, body, bodiesAABBoxHitBody);

        return bodiesAABBoxHitBody;
    }

}
