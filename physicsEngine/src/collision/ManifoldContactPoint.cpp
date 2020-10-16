#include "collision/ManifoldContactPoint.h"

namespace urchin {

    ManifoldContactPoint::ManifoldContactPoint() :
            normalFromObject2(Vector3<float>()),
            pointOnObject1(Point3<float>()),
            pointOnObject2(Point3<float>()),
            localPointOnObject1(Point3<float>()),
            localPointOnObject2(Point3<float>()),
            depth(0.0),
            bIsPredictive(false) {

    }

    /**
    * @param normalFromObject2 Contact normal from object 2. The normal direction should be toward the object 1.
    * @param depth Penetration depth (negative when collision exist)
    */
    ManifoldContactPoint::ManifoldContactPoint(const Vector3<float> &normalFromObject2,
            const Point3<float> &pointOnObject1,
            const Point3<float> &pointOnObject2,
            const Point3<float> &localPointOnObject1,
            const Point3<float> &localPointOnObject2,
            float depth,
            bool bIsPredictive) :
            normalFromObject2(normalFromObject2),
            pointOnObject1(pointOnObject1),
            pointOnObject2(pointOnObject2),
            localPointOnObject1(localPointOnObject1),
            localPointOnObject2(localPointOnObject2),
            depth(depth),
            bIsPredictive(bIsPredictive) {

    }

    /**
     * @return Contact normal starting from object 2
     */
    const Vector3<float> &ManifoldContactPoint::getNormalFromObject2() const {
        return normalFromObject2;
    }

    /**
     * @return Contact point on object 1 in world position
     */
    const Point3<float> &ManifoldContactPoint::getPointOnObject1() const {
        return pointOnObject1;
    }

    /**
     * @return Contact point on object 2 in world position
     */
    const Point3<float> &ManifoldContactPoint::getPointOnObject2() const {
        return pointOnObject2;
    }

    /**
     * @return Local contact point on object 1
     */
    const Point3<float> &ManifoldContactPoint::getLocalPointOnObject1() const {
        return localPointOnObject1;
    }

    /**
     * @return Local contact point on object 2
     */
    const Point3<float> &ManifoldContactPoint::getLocalPointOnObject2() const {
        return localPointOnObject2;
    }

    /**
     * @return Penetration depth (negative when collision exist)
     */
    float ManifoldContactPoint::getDepth() const {
        return depth;
    }

    /**
     * @return True if contact point is predictive
     */
    bool ManifoldContactPoint::isPredictive() const {
        return bIsPredictive;
    }

    /**
     * Updates contact points in world position
     * @param pointOnObject1 Contact point on object 1 in world position
     * @param pointOnObject2 Contact point on object 2 in world position
     */
    void ManifoldContactPoint::updatePoints(const Point3<float> &pointOnObject1, const Point3<float> &pointOnObject2) {
        this->pointOnObject1 = pointOnObject1;
        this->pointOnObject2 = pointOnObject2;
    }

    /**
     * @param depth Penetration depth (negative when collision exist)
     */
    void ManifoldContactPoint::updateDepth(float depth) {
        this->depth = depth;
    }

    /**
     * @return Accumulated data stored by constraint solver through frames
     */
    AccumulatedSolvingData &ManifoldContactPoint::getAccumulatedSolvingData() {
        return accumulatedSolvingData;
    }
}
