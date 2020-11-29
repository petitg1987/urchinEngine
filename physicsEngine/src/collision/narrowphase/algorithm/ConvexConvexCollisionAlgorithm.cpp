#include <memory>

#include "collision/narrowphase/algorithm/ConvexConvexCollisionAlgorithm.h"
#include "object/CollisionConvexObject3D.h"

namespace urchin {

    ConvexConvexCollisionAlgorithm::ConvexConvexCollisionAlgorithm(bool objectSwapped, ManifoldResult &&result) :
            CollisionAlgorithm(objectSwapped, std::move(result)) {

    }

    void ConvexConvexCollisionAlgorithm::doProcessCollisionAlgorithm(const CollisionObjectWrapper& object1, const CollisionObjectWrapper& object2) {
        ScopeProfiler profiler("physics", "algConvConv");

        //transform convex hull shapes
        std::unique_ptr<CollisionConvexObject3D, ObjectDeleter> convexObject1 = object1.getShape().toConvexObject(object1.getShapeWorldTransform());
        std::unique_ptr<CollisionConvexObject3D, ObjectDeleter> convexObject2 = object2.getShape().toConvexObject(object2.getShapeWorldTransform());

        //process GJK and EPA hybrid algorithms
        std::unique_ptr<GJKResult<double>, AlgorithmResultDeleter> gjkResultWithoutMargin = gjkAlgorithm.processGJK(*convexObject1, *convexObject2, false);

        if (gjkResultWithoutMargin->isValidResult()) {
            if (gjkResultWithoutMargin->isCollide()) { //collision detected on reduced objects (without margins)
                processCollisionAlgorithmWithMargin(convexObject1, convexObject2);
            } else { //collision detected on enlarged objects (with margins) OR no collision detected
                const Vector3<double> &vectorBA = gjkResultWithoutMargin->getClosestPointB().vector(gjkResultWithoutMargin->getClosestPointA());
                float vectorBALength = vectorBA.length();
                float sumMargins = convexObject1->getOuterMargin() + convexObject2->getOuterMargin();
                if (sumMargins > vectorBALength - getContactBreakingThreshold()) { //collision detected on enlarged objects
                    const Vector3<double> &normalFromObject2 = vectorBA.normalize();
                    const Point3<double> &pointOnObject2 = gjkResultWithoutMargin->getClosestPointB().translate(normalFromObject2 * (double)convexObject2->getOuterMargin());
                    const float penetrationDepth = vectorBALength - sumMargins;

                    addNewContactPoint(normalFromObject2.cast<float>(), pointOnObject2.cast<float>(), penetrationDepth);
                }
            }
        }
    }

    void ConvexConvexCollisionAlgorithm::processCollisionAlgorithmWithMargin(const std::unique_ptr<CollisionConvexObject3D, ObjectDeleter>& convexObject1,
            const std::unique_ptr<CollisionConvexObject3D, ObjectDeleter>& convexObject2) {
        std::unique_ptr<GJKResult<double>, AlgorithmResultDeleter> gjkResultWithMargin = gjkAlgorithm.processGJK(*convexObject1, *convexObject2, true);

        if (gjkResultWithMargin->isValidResult() && gjkResultWithMargin->isCollide()) {
            std::unique_ptr<EPAResult<double>, AlgorithmResultDeleter> epaResult = epaAlgorithm.processEPA(*convexObject1, *convexObject2, *gjkResultWithMargin);

            if (epaResult->isValidResult() && epaResult->isCollide()) { //should be always true except for problems due to float imprecision
                const Vector3<double> &normalFromObject2 = (-epaResult->getNormal());
                const Point3<double> &pointOnObject2 = epaResult->getContactPointB();
                const float penetrationDepth = -epaResult->getPenetrationDepth();

                addNewContactPoint(normalFromObject2.cast<float>(), pointOnObject2.cast<float>(), penetrationDepth);
            }
        }
    }

    CollisionAlgorithm *ConvexConvexCollisionAlgorithm::Builder::createCollisionAlgorithm(bool objectSwapped, ManifoldResult &&result, FixedSizePool<CollisionAlgorithm>* algorithmPool) const {
        void *memPtr = algorithmPool->allocate(sizeof(ConvexConvexCollisionAlgorithm));
        return new(memPtr) ConvexConvexCollisionAlgorithm(objectSwapped, std::move(result));
    }

    const std::vector<CollisionShape3D::ShapeType> &ConvexConvexCollisionAlgorithm::Builder::getFirstExpectedShapeType() const {
        return CollisionShape3D::CONVEX_SHAPES;
    }

    unsigned int ConvexConvexCollisionAlgorithm::Builder::getAlgorithmSize() const {
        return sizeof(ConvexConvexCollisionAlgorithm);
    }

}
