#include <memory>

#include "collision/narrowphase/algorithm/ConvexConvexCollisionAlgorithm.h"
#include "collision/narrowphase/algorithm/gjk/GJKConvexObjectWrapper.h"
#include "object/CollisionConvexObject3D.h"

namespace urchin {

    ConvexConvexCollisionAlgorithm::ConvexConvexCollisionAlgorithm(bool objectSwapped, const ManifoldResult& result) :
            CollisionAlgorithm(objectSwapped, result) {

    }

    void ConvexConvexCollisionAlgorithm::doProcessCollisionAlgorithm(const CollisionObjectWrapper& object1, const CollisionObjectWrapper& object2) {
        ScopeProfiler sp(Profiler::physics(), "algConvConv");

        //transform convex hull shapes
        std::unique_ptr<CollisionConvexObject3D, ObjectDeleter> convexObject1 = object1.getShape().toConvexObject(object1.getShapeWorldTransform());
        std::unique_ptr<CollisionConvexObject3D, ObjectDeleter> convexObject2 = object2.getShape().toConvexObject(object2.getShapeWorldTransform());

        //process GJK and EPA hybrid algorithms
        GJKResult<double> gjkResultWithoutMargin = gjkAlgorithm.processGJK(GJKConvexObjectWrapper(*convexObject1, false), GJKConvexObjectWrapper(*convexObject2, false));

        if (gjkResultWithoutMargin.isValidResult()) {
            if (gjkResultWithoutMargin.isCollide()) { //collision detected on reduced objects (without margins)
                processCollisionAlgorithmWithMargin(*convexObject1, *convexObject2);
            } else { //collision detected on enlarged objects (with margins) OR no collision detected
                Vector3<double> vectorBA = gjkResultWithoutMargin.getClosestPointB().vector(gjkResultWithoutMargin.getClosestPointA());
                auto vectorBALength = (float)vectorBA.length();
                float sumMargins = convexObject1->getOuterMargin() + convexObject2->getOuterMargin();
                if (sumMargins > vectorBALength - getContactBreakingThreshold()) { //collision detected on enlarged objects
                    Vector3<double> normalFromObject2 = vectorBA.normalize();
                    Point3<double> pointOnObject2 = gjkResultWithoutMargin.getClosestPointB().translate(normalFromObject2 * (double)convexObject2->getOuterMargin());
                    const float penetrationDepth = vectorBALength - sumMargins;

                    addNewContactPoint(normalFromObject2.cast<float>(), pointOnObject2.cast<float>(), penetrationDepth);
                }
            }
        }
    }

    void ConvexConvexCollisionAlgorithm::processCollisionAlgorithmWithMargin(const CollisionConvexObject3D& convexObject1, const CollisionConvexObject3D& convexObject2) {
        GJKResult<double> gjkResultWithMargin = gjkAlgorithm.processGJK(GJKConvexObjectWrapper(convexObject1, true), GJKConvexObjectWrapper(convexObject2, true));

        if (gjkResultWithMargin.isValidResult() && gjkResultWithMargin.isCollide()) {
            EPAResult<double> epaResult = epaAlgorithm.processEPA(convexObject1, convexObject2, gjkResultWithMargin);

            if (epaResult.isValidResult() && epaResult.isCollide()) { //should be always true except for problems due to float imprecision
                const Vector3<double>& normalFromObject2 = (-epaResult.getNormal());
                const Point3<double>& pointOnObject2 = epaResult.getContactPointB();
                auto penetrationDepth = (float)-epaResult.getPenetrationDepth();

                addNewContactPoint(normalFromObject2.cast<float>(), pointOnObject2.cast<float>(), penetrationDepth);
            }
        }
    }

    CollisionAlgorithm* ConvexConvexCollisionAlgorithm::Builder::createCollisionAlgorithm(bool objectSwapped, const ManifoldResult& result, FixedSizePool<CollisionAlgorithm>& algorithmPool) const {
        void* memPtr = algorithmPool.allocate(sizeof(ConvexConvexCollisionAlgorithm));
        return new(memPtr) ConvexConvexCollisionAlgorithm(objectSwapped, result);
    }

    const std::vector<CollisionShape3D::ShapeType>& ConvexConvexCollisionAlgorithm::Builder::getFirstExpectedShapeType() const {
        return CollisionShape3D::convexShapes();
    }

    unsigned int ConvexConvexCollisionAlgorithm::Builder::getAlgorithmSize() const {
        return sizeof(ConvexConvexCollisionAlgorithm);
    }

}
