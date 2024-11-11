#include <limits>
#include <cmath>
#include <cassert>

#include <shape/CollisionConvexHullShape.h>
#include <object/CollisionConvexHullObject.h>

namespace urchin {

    //static
    thread_local std::unordered_map<const CollisionConvexHullShape*, AABBox<float>> CollisionConvexHullShape::aabboxCache;
    thread_local std::unordered_map<const CollisionConvexHullShape*, PhysicsTransform> CollisionConvexHullShape::transformCache;

    /**
    * @param points Points used to construct the convex hull
    */
    CollisionConvexHullShape::CollisionConvexHullShape(const std::vector<Point3<float>>& points) :
            convexHullShape(std::make_unique<ConvexHullShape3D<float>>(points)),
            minDistanceToCenter(0.0f),
            maxDistanceToCenter(0.0f) {
        initialize();
    }

    CollisionConvexHullShape::CollisionConvexHullShape(std::unique_ptr<ConvexHullShape3D<float>> convexHullShape) :
            convexHullShape(std::move(convexHullShape)),
            minDistanceToCenter(0.0f),
            maxDistanceToCenter(0.0f) {
        initialize();
    }

    CollisionConvexHullShape::CollisionConvexHullShape(CollisionConvexHullShape&& collisionConvexHullShape) noexcept :
            CollisionShape3D(collisionConvexHullShape),
            convexHullShape(std::exchange(collisionConvexHullShape.convexHullShape, nullptr)),
            convexHullShapeReduced(std::move(collisionConvexHullShape.convexHullShapeReduced)),
            minDistanceToCenter(std::exchange(collisionConvexHullShape.minDistanceToCenter, 0.0f)),
            maxDistanceToCenter(std::exchange(collisionConvexHullShape.maxDistanceToCenter, 0.0f)) {
        transformCache[this] = PhysicsTransform(Point3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()));
    }

    CollisionConvexHullShape::~CollisionConvexHullShape() {
        aabboxCache.erase(this);
        transformCache.erase(this);
    }

    void CollisionConvexHullShape::initialize() {
        transformCache[this] = PhysicsTransform(Point3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()));

        initializeDistances();
        initializeConvexHullReduced();
    }

    void CollisionConvexHullShape::initializeConvexHullReduced() {
        if (minDistanceToCenter > getInnerMargin()) {
            convexHullShapeReduced = convexHullShape->resize(-getInnerMargin());
        } else { //impossible to shrink the convex hull correctly
            refreshInnerMargin(0.0f);
        }
    }

    void CollisionConvexHullShape::initializeDistances() {
        AABBox<float> aabbox = toAABBox(PhysicsTransform());
        maxDistanceToCenter = aabbox.getMaxHalfSize();
        minDistanceToCenter = aabbox.getMinHalfSize();
    }

    CollisionShape3D::ShapeType CollisionConvexHullShape::getShapeType() const {
        return CONVEX_HULL_SHAPE;
    }

    const ConvexShape3D<float>& CollisionConvexHullShape::getSingleShape() const {
        return *convexHullShape;
    }

    /**
     * @return Points including margin of the convex hull
     */
    std::vector<Point3<float>> CollisionConvexHullShape::getPoints() const {
        return convexHullShape->getPoints();
    }

    std::unique_ptr<CollisionShape3D> CollisionConvexHullShape::scale(const Vector3<float>& scale) const {
        const std::vector<Point3<float>>& convexHullPoints = convexHullShape->getPoints();

        std::vector<Point3<float>> newPoints;
        newPoints.reserve(convexHullPoints.size());

        for (const auto& convexHullPoint : convexHullPoints) {
            newPoints.emplace_back(convexHullPoint.X * scale.X, convexHullPoint.Y * scale.Y, convexHullPoint.Z * scale.Z);
        }

        return std::make_unique<CollisionConvexHullShape>(newPoints);
    }

    AABBox<float> CollisionConvexHullShape::toAABBox(const PhysicsTransform& physicsTransform) const {
        PhysicsTransform& lastTransform = transformCache[this];
        AABBox<float> &aabbox = aabboxCache[this];

        if (!lastTransform.equals(physicsTransform)) {
            const Quaternion<float>& orientation = physicsTransform.getOrientation();
            Point3 min(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
            Point3 max(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
            const std::vector<Point3<float>>& convexHullShapePoints = convexHullShape->getPoints();
            for (const auto& convexHullShapePoint : convexHullShapePoints) {
                const Point3<float> point = orientation.rotatePoint(convexHullShapePoint);

                min.X = std::min(min.X, point.X);
                min.Y = std::min(min.Y, point.Y);
                min.Z = std::min(min.Z, point.Z);

                max.X = std::max(max.X, point.X);
                max.Y = std::max(max.Y, point.Y);
                max.Z = std::max(max.Z, point.Z);
            }

            const Point3<float>& position = physicsTransform.getPosition();

            aabbox = AABBox(min + position, max + position);
            lastTransform = physicsTransform;
        }

        return aabbox;
    }

    std::unique_ptr<CollisionConvexObject3D, ObjectDeleter> CollisionConvexHullShape::toConvexObject(const PhysicsTransform& physicsTransform) const {
        Transform<float> transform = physicsTransform.toTransform();
        auto convexHullWithMargin = std::unique_ptr<ConvexHull3D<float>>(static_cast<ConvexHull3D<float>*>(convexHullShape->toConvexObject(transform).release()));

        void* memPtr = getObjectsPool().allocate(sizeof(CollisionConvexHullObject));

        if (!convexHullShapeReduced) { //impossible to compute convex hull without margin => use convex hull with margin and set a margin of 0.0
            assert(getInnerMargin() == 0.0f);
            auto* collisionObjectPtr = new (memPtr) CollisionConvexHullObject(std::move(convexHullWithMargin));
            return std::unique_ptr<CollisionConvexHullObject, ObjectDeleter>(collisionObjectPtr);
        }

        auto convexHullWithoutMargin = std::unique_ptr<ConvexHull3D<float>>(static_cast<ConvexHull3D<float>*>(convexHullShapeReduced->toConvexObject(transform).release()));
        auto* collisionObjectPtr = new (memPtr) CollisionConvexHullObject(getInnerMargin(), std::move(convexHullWithMargin), std::move(convexHullWithoutMargin));
        return std::unique_ptr<CollisionConvexHullObject, ObjectDeleter>(collisionObjectPtr);
    }

    Vector3<float> CollisionConvexHullShape::computeLocalInertia(float mass) const {
        AABBox<float> aabbox = toAABBox(PhysicsTransform());
        float width = 2.0f * aabbox.getHalfSize(0);
        float height = 2.0f * aabbox.getHalfSize(1);
        float depth = 2.0f * aabbox.getHalfSize(2);

        float localInertia1 = (1.0f / 12.0f) * mass * (height * height + depth * depth);
        float localInertia2 = (1.0f / 12.0f) * mass * (width * width + depth * depth);
        float localInertia3 = (1.0f / 12.0f) * mass * (width * width + height * height);
        return Vector3(localInertia1, localInertia2, localInertia3);
    }

    float CollisionConvexHullShape::getMaxDistanceToCenter() const {
        return maxDistanceToCenter;
    }

    float CollisionConvexHullShape::getMinDistanceToCenter() const {
        return minDistanceToCenter;
    }

    std::unique_ptr<CollisionShape3D> CollisionConvexHullShape::clone() const {
        return std::make_unique<CollisionConvexHullShape>(convexHullShape->getPoints());
    }

}
