#include <vector>

#include "collision/narrowphase/algorithm/ConcaveAnyCollisionAlgorithm.h"
#include "collision/narrowphase/algorithm/CollisionAlgorithmSelector.h"
#include "shape/CollisionTriangleShape.h"
#include "shape/CollisionConcaveShape.h"

namespace urchin {

    ConcaveAnyCollisionAlgorithm::ConcaveAnyCollisionAlgorithm(bool objectSwapped, const ManifoldResult& result) :
            CollisionAlgorithm(objectSwapped, result) {

    }

    void ConcaveAnyCollisionAlgorithm::doProcessCollisionAlgorithm(const CollisionObjectWrapper& object1, const CollisionObjectWrapper& object2) {
        ScopeProfiler sp(Profiler::physics(), "algConcaveAny");

        const CollisionShape3D& otherShape = object2.getShape();

        AbstractBody& body1 = getManifoldResult().getBody1();
        AbstractBody& body2 = getManifoldResult().getBody2();

        AABBox<float> aabboxLocalToObject1 = object2.getShape().toAABBox(object1.getShapeWorldTransform().inverse() * object2.getShapeWorldTransform());
        const auto& concaveShape = dynamic_cast<const CollisionConcaveShape&>(object1.getShape());

        const std::vector<CollisionTriangleShape>& triangles = concaveShape.findTrianglesInAABBox(aabboxLocalToObject1);
        for (const auto& triangle : triangles) {
            auto collisionAlgorithm = getCollisionAlgorithmSelector()->createCollisionAlgorithm(body1, triangle, body2, otherShape);

            CollisionObjectWrapper subObject1(triangle, object1.getShapeWorldTransform());
            CollisionObjectWrapper subObject2(otherShape, object2.getShapeWorldTransform());

            collisionAlgorithm->processCollisionAlgorithm(subObject1, subObject2, false);

            const ManifoldResult& algorithmManifoldResult = collisionAlgorithm->getConstManifoldResult();
            addContactPointsToManifold(algorithmManifoldResult, collisionAlgorithm->isObjectSwapped());
        }
    }

    void ConcaveAnyCollisionAlgorithm::addContactPointsToManifold(const ManifoldResult& manifoldResult, bool manifoldSwapped) {
        for (unsigned int i = 0; i < manifoldResult.getNumContactPoints(); ++i) {
            const ManifoldContactPoint& manifoldContactPoint = manifoldResult.getManifoldContactPoint(i);
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

    CollisionAlgorithm* ConcaveAnyCollisionAlgorithm::Builder::createCollisionAlgorithm(bool objectSwapped, const ManifoldResult& result, FixedSizePool<CollisionAlgorithm>& algorithmPool) const {
        void* memPtr = algorithmPool.allocate(sizeof(ConcaveAnyCollisionAlgorithm));
        return new(memPtr) ConcaveAnyCollisionAlgorithm(objectSwapped, result);
    }

    const std::vector<CollisionShape3D::ShapeType>& ConcaveAnyCollisionAlgorithm::Builder::getFirstExpectedShapeType() const {
        return CollisionShape3D::concaveShapes();
    }

    unsigned int ConcaveAnyCollisionAlgorithm::Builder::getAlgorithmSize() const {
        return sizeof(ConcaveAnyCollisionAlgorithm);
    }

}
