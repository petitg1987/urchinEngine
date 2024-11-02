#include <sstream>

#include <shape/CollisionShape3D.h>
#include <utils/property/EagerPropertyLoader.h>
#include <object/pool/CollisionConvexObjectPool.h>

namespace urchin {

    CollisionShape3D::CollisionShape3D() :
            innerMargin(EagerPropertyLoader::getCollisionShapeInnerMargin()),
            initialInnerMargin(innerMargin) {

    }

    CollisionShape3D::CollisionShape3D(float innerMargin) :
            innerMargin(innerMargin),
            initialInnerMargin(innerMargin) {

    }

    const std::vector<CollisionShape3D::ShapeType>& CollisionShape3D::convexShapes() {
        static std::vector convexShapes = {TRIANGLE_SHAPE, SPHERE_SHAPE, CAPSULE_SHAPE, CYLINDER_SHAPE, BOX_SHAPE, CONVEX_HULL_SHAPE, CONE_SHAPE};
        return convexShapes;
    }

    const std::vector<CollisionShape3D::ShapeType>& CollisionShape3D::concaveShapes() {
        static std::vector concaveShapes = {HEIGHTFIELD_SHAPE};
        return concaveShapes;
    }

    const std::vector<CollisionShape3D::ShapeType>& CollisionShape3D::compoundShapes() {
        static std::vector compoundShapes = {COMPOUND_SHAPE};
        return compoundShapes;
    }

    const std::vector<CollisionShape3D::ShapeType>& CollisionShape3D::sphereShapes() {
        static std::vector sphereShapes = {SPHERE_SHAPE};
        return sphereShapes;
    }

    FixedSizePool<CollisionConvexObject3D>& CollisionShape3D::getObjectsPool() {
        return CollisionConvexObjectPool::instance().getObjectsPool();
    }

    void CollisionShape3D::refreshInnerMargin(float maximumInnerMargin) {
        if (this->innerMargin > maximumInnerMargin) {
            this->innerMargin = maximumInnerMargin;
        }
    }

    float CollisionShape3D::getInnerMargin() const {
        return innerMargin;
    }

    void CollisionShape3D::checkInnerMarginQuality(std::string_view shapeId) const {
        if (initialInnerMargin > innerMargin) {
            constexpr float RELATIVE_MARGIN_FACTOR_BIG_SHAPE = 200.0f;
            AABBox<float> aabbox = toAABBox(PhysicsTransform());
            float shapeLength = aabbox.getMin().vector(aabbox.getMax()).length();
            bool isBigShape = shapeLength > initialInnerMargin * RELATIVE_MARGIN_FACTOR_BIG_SHAPE;

            if (isBigShape) {
                std::stringstream logStream;
                logStream << "Not optimal margin on shape id " << shapeId << "." << std::endl;
                logStream << " - Current margin: " << innerMargin << std::endl;
                logStream << " - Expected margin: " << initialInnerMargin << std::endl;
                logStream << " - Shape length: " << shapeLength;
                Logger::instance().logWarning(logStream.str());
            }
        }
    }

    bool CollisionShape3D::isConvex() const {
        return std::ranges::find(convexShapes(), getShapeType()) != convexShapes().end();
    }

    bool CollisionShape3D::isConcave() const {
        return std::ranges::find(concaveShapes(), getShapeType()) != concaveShapes().end();
    }

    bool CollisionShape3D::isCompound() const {
        return std::ranges::find(compoundShapes(), getShapeType()) != compoundShapes().end();
    }
}
