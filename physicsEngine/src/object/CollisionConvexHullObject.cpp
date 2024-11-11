#include <sstream>
#include <utility>

#include <object/CollisionConvexHullObject.h>

namespace urchin {

    /**
     * @param pointsWithMargin Points including margin used to construct the convex hull. Points inside the convex hull are accepted but will unused.
     * @param pointsWithoutMargin Points without margin used to construct the convex hull. Points inside the convex hull are accepted but will unused.
     * @param outerMargin Collision outer margin. Collision margin must match with convex hulls arguments.
     */
    CollisionConvexHullObject::CollisionConvexHullObject(float outerMargin, const std::vector<Point3<float>>& pointsWithMargin, const std::vector<Point3<float>>& pointsWithoutMargin) :
            CollisionConvexObject3D(outerMargin),
            convexHullObjectWithMargin(std::make_unique<ConvexHull3D<float>>(pointsWithMargin)),
            convexHullObjectWithoutMargin(std::make_unique<ConvexHull3D<float>>(pointsWithoutMargin)) {

    }

    /**
     * @param outerMargin Collision outer margin. Collision margin must match with convex hulls arguments.
     */
    CollisionConvexHullObject::CollisionConvexHullObject(float outerMargin, std::unique_ptr<ConvexHull3D<float>> convexHullObjectWithMargin, std::unique_ptr<ConvexHull3D<float>> convexHullObjectWithoutMargin) :
            CollisionConvexObject3D(outerMargin),
            convexHullObjectWithMargin(std::move(convexHullObjectWithMargin)),
            convexHullObjectWithoutMargin(std::move(convexHullObjectWithoutMargin)) {

    }

    /**
     * Special constructor when convexHullObjectWithoutMargin is not available. The convexHullObjectWithMargin must be used instead and outer margin is 0.0.
     */
    CollisionConvexHullObject::CollisionConvexHullObject(std::unique_ptr<ConvexHull3D<float>> convexHullObjectWithMargin) :
            CollisionConvexObject3D(0.0f),
            convexHullObjectWithMargin(std::move(convexHullObjectWithMargin)),
            convexHullObjectWithoutMargin(std::unique_ptr<ConvexHull3D<float>>(nullptr)) {

    }

    ConvexHull3D<float>& CollisionConvexHullObject::getConvexHullWithoutMargin() const {
        if (!convexHullObjectWithoutMargin) {
            return getConvexHullWithMargin();
        }
        return *convexHullObjectWithoutMargin;
    }

    ConvexHull3D<float>& CollisionConvexHullObject::getConvexHullWithMargin() const {
        return *convexHullObjectWithMargin;
    }

    CollisionConvexObject3D::ObjectType CollisionConvexHullObject::getObjectType() const {
        return CONVEX_HULL_OBJECT;
    }

    /**
     * @return includeMargin Indicate whether support function need to take into account margin
     */
    Point3<float> CollisionConvexHullObject::getSupportPoint(const Vector3<float>& direction, bool includeMargin) const {
        if (includeMargin || !convexHullObjectWithoutMargin) {
            return convexHullObjectWithMargin->getSupportPoint(direction);
        }

        return convexHullObjectWithoutMargin->getSupportPoint(direction);
    }

    ConvexHull3D<float> CollisionConvexHullObject::retrieveConvexHull() const {
        return ConvexHull3D(*convexHullObjectWithMargin);
    }

    std::string CollisionConvexHullObject::toString() const {
        std::stringstream ss;
        ss.precision(std::numeric_limits<float>::max_digits10);

        ss << "Collision convex hull:" << std::endl;
        ss << std::setw(20) << std::left << " - Outer margin: " << getOuterMargin() << std::endl;
        ss << std::setw(20) << std::left << " - Convex hull (margin): " << (*convexHullObjectWithMargin) << std::endl;
        if (convexHullObjectWithoutMargin) {
            ss << std::setw(20) << std::left << " - Convex hull (no margin): " << (*convexHullObjectWithoutMargin);
        }

        return ss.str();
    }

}
