#include <limits>

#include <collision/narrowphase/algorithm/SphereBoxCollisionAlgorithm.h>
#include <shape/CollisionSphereShape.h>
#include <shape/CollisionBoxShape.h>

namespace urchin {

    SphereBoxCollisionAlgorithm::SphereBoxCollisionAlgorithm(bool objectSwapped, const ManifoldResult& result) :
            CollisionAlgorithm(objectSwapped, result) {

    }

    void SphereBoxCollisionAlgorithm::doProcessCollisionAlgorithm(const CollisionObjectWrapper& object1, const CollisionObjectWrapper& object2) {
        ScopeProfiler sp(Profiler::physics(), "algSphereBox");

        const auto& sphere1 = dynamic_cast<const CollisionSphereShape&>(object1.getShape());
        const auto& box2 = dynamic_cast<const CollisionBoxShape&>(object2.getShape());

        //For easiest computation, we need to have box position on origin.
        //So, we transform sphere position into box local space.
        const Point3<float>& spherePos = object1.getShapeWorldTransform().getPosition();
        const Point3<float>& spherePosLocalBox = object2.getShapeWorldTransform().inverseTransform(spherePos);

        Point3<float> closestPointOnBox(
                MathFunction::clamp(spherePosLocalBox.X, -box2.getHalfSize(0), box2.getHalfSize(0)),
                MathFunction::clamp(spherePosLocalBox.Y, -box2.getHalfSize(1), box2.getHalfSize(1)),
                MathFunction::clamp(spherePosLocalBox.Z, -box2.getHalfSize(2), box2.getHalfSize(2)) );

        Vector3<float> normalFromObject2 = closestPointOnBox.vector(spherePosLocalBox);

        float boxSphereLength = normalFromObject2.length();
        if (boxSphereLength - getContactBreakingThreshold() < sphere1.getRadius()) { //collision detected

            float depth;

            if (boxSphereLength > std::numeric_limits<float>::epsilon()) { //sphere position is outside the box

                //compute depth penetration
                boxSphereLength = normalFromObject2.length();
                depth = boxSphereLength - sphere1.getRadius();
            } else { //special case when sphere position is inside the box: closestPointOnBox == spherePosLocalBox

                //find axis closest to sphere position
                float minDistToAxis = box2.getHalfSize(0) - std::abs(spherePosLocalBox[0]);
                std::size_t minAxis = 0;
                for (std::size_t i = 1; i < 3; ++i) {
                    float distToAxis = box2.getHalfSize((unsigned int)i) - std::abs(spherePosLocalBox[i]);
                    if (distToAxis < minDistToAxis) {
                        minDistToAxis = distToAxis;
                        minAxis = i;
                    }
                }

                //project sphere point on found axis
                closestPointOnBox[minAxis] = spherePosLocalBox[minAxis] + minDistToAxis * MathFunction::sign<float>(spherePosLocalBox[minAxis]);

                //normal computation
                normalFromObject2 = spherePosLocalBox.vector(closestPointOnBox);

                //compute depth penetration
                boxSphereLength = normalFromObject2.length();
                depth = -(boxSphereLength + sphere1.getRadius());
            }

            //normalize normal
            normalFromObject2 /= boxSphereLength;

            //transform back in world space
            closestPointOnBox = object2.getShapeWorldTransform().transform(closestPointOnBox);
            normalFromObject2 = object2.getShapeWorldTransform().getOrientation().rotateVector(normalFromObject2);

            addNewContactPoint(normalFromObject2, closestPointOnBox, depth);
        }
    }

    CollisionAlgorithm* SphereBoxCollisionAlgorithm::Builder::createCollisionAlgorithm(bool objectSwapped, const ManifoldResult& result, FixedSizePool<CollisionAlgorithm>& algorithmPool) const {
        void* memPtr = algorithmPool.allocate(sizeof(SphereBoxCollisionAlgorithm));
        return new(memPtr) SphereBoxCollisionAlgorithm(objectSwapped, result);
    }

    const std::vector<CollisionShape3D::ShapeType>& SphereBoxCollisionAlgorithm::Builder::getFirstExpectedShapeType() const {
        return CollisionShape3D::sphereShapes();
    }

    unsigned int SphereBoxCollisionAlgorithm::Builder::getAlgorithmSize() const {
        return sizeof(SphereBoxCollisionAlgorithm);
    }

}
