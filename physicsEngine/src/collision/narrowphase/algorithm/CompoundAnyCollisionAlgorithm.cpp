#include "collision/narrowphase/algorithm/CompoundAnyCollisionAlgorithm.h"
#include "shape/CollisionCompoundShape.h"
#include "shape/CollisionShape3D.h"

namespace urchin {

    CompoundAnyCollisionAlgorithm::CompoundAnyCollisionAlgorithm(bool objectSwapped, ManifoldResult &&result) :
            CollisionAlgorithm(objectSwapped, std::move(result)) {

    }

    void CompoundAnyCollisionAlgorithm::doProcessCollisionAlgorithm(const CollisionObjectWrapper& object1, const CollisionObjectWrapper& object2) {
        ScopeProfiler profiler("physics", "algCompoundAny");

        const auto &compoundShape = dynamic_cast<const CollisionCompoundShape &>(object1.getShape());
        const CollisionShape3D &otherShape = object2.getShape();

        AbstractWorkBody *body1 = getManifoldResult().getBody1();
        AbstractWorkBody *body2 = getManifoldResult().getBody2();

        const std::vector<std::shared_ptr<const LocalizedCollisionShape>> &localizedShapes = compoundShape.getLocalizedShapes();
        for (const auto &localizedShape : localizedShapes) {
            std::shared_ptr<CollisionAlgorithm> collisionAlgorithm = getCollisionAlgorithmSelector()->createCollisionAlgorithm(
                    body1, localizedShape->shape.get(), body2, &otherShape);

            PhysicsTransform shapeWorldTransform = object1.getShapeWorldTransform() * localizedShape->transform;
            CollisionObjectWrapper subObject1(*localizedShape->shape, shapeWorldTransform);
            CollisionObjectWrapper subObject2(otherShape, object2.getShapeWorldTransform());

            collisionAlgorithm->processCollisionAlgorithm(subObject1, subObject2, false);

            const ManifoldResult &algorithmManifoldResult = collisionAlgorithm->getConstManifoldResult();
            addContactPointsToManifold(algorithmManifoldResult, collisionAlgorithm->isObjectSwapped());
        }
    }

    void CompoundAnyCollisionAlgorithm::addContactPointsToManifold(const ManifoldResult& manifoldResult, bool manifoldSwapped) {
        for (unsigned int i=0; i<manifoldResult.getNumContactPoints(); ++i) {
            const ManifoldContactPoint &manifoldContactPoint = manifoldResult.getManifoldContactPoint(i);
            if (manifoldSwapped) {
                getManifoldResult().addContactPoint(
                        (-manifoldContactPoint.getNormalFromObject2()),
                        manifoldContactPoint.getPointOnObject2(),
                        manifoldContactPoint.getPointOnObject1(),
                        manifoldContactPoint.getLocalPointOnObject2(),
                        manifoldContactPoint.getLocalPointOnObject1(),
                        manifoldContactPoint.getDepth(),
                        manifoldContactPoint.isPredictive());
            } else {
                getManifoldResult().addContactPoint(
                        manifoldContactPoint.getNormalFromObject2(),
                        manifoldContactPoint.getPointOnObject1(),
                        manifoldContactPoint.getPointOnObject2(),
                        manifoldContactPoint.getLocalPointOnObject1(),
                        manifoldContactPoint.getLocalPointOnObject2(),
                        manifoldContactPoint.getDepth(),
                        manifoldContactPoint.isPredictive());
            }
        }
    }

    CollisionAlgorithm *CompoundAnyCollisionAlgorithm::Builder::createCollisionAlgorithm(bool objectSwapped, ManifoldResult &&result, FixedSizePool<CollisionAlgorithm>* algorithmPool) const {
        void *memPtr = algorithmPool->allocate(sizeof(CompoundAnyCollisionAlgorithm));
        return new(memPtr) CompoundAnyCollisionAlgorithm(objectSwapped, std::move(result));
    }

    const std::vector<CollisionShape3D::ShapeType> &CompoundAnyCollisionAlgorithm::Builder::getFirstExpectedShapeType() const {
        return CollisionShape3D::COMPOUND_SHAPES;
    }

    unsigned int CompoundAnyCollisionAlgorithm::Builder::getAlgorithmSize() const {
        return sizeof(CompoundAnyCollisionAlgorithm);
    }

}
