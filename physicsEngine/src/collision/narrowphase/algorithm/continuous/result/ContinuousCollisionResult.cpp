#include "ContinuousCollisionResult.h"

namespace urchin {

    template<class T> ContinuousCollisionResult<T>::ContinuousCollisionResult(AbstractWorkBody *body2, const Vector3<T> &normalFromObject2,
            const Point3<T> &hitPointOnObject2, T timeToHit) :
        body2(body2),
        normalFromObject2(normalFromObject2),
        hitPointOnObject2(hitPointOnObject2),
        timeToHit(timeToHit) {

    }

    template<class T> ContinuousCollisionResult<T>::ContinuousCollisionResult(const ContinuousCollisionResult &continuousCollisionResult) :
            body2(continuousCollisionResult.getBody2()),
            normalFromObject2(continuousCollisionResult.getNormalFromObject2()),
            hitPointOnObject2(continuousCollisionResult.getHitPointOnObject2()),
            timeToHit(continuousCollisionResult.getTimeToHit()) {

    }

    template<class T> AbstractWorkBody *ContinuousCollisionResult<T>::getBody2() const {
        return body2;
    }

    template<class T> const Vector3<T> &ContinuousCollisionResult<T>::getNormalFromObject2() const {
        return normalFromObject2;
    }

    template<class T> const Point3<T> &ContinuousCollisionResult<T>::getHitPointOnObject2() const {
        return hitPointOnObject2;
    }

    /**
     * Return time to hit. A value of 0.0 means objects collide at initial situation (from transformation).
     * A value of 1.0 means objects will collide at final situation (to transformation).
     */
    template<class T> T ContinuousCollisionResult<T>::getTimeToHit() const {
        return timeToHit;
    }

    template<class T> bool ContinuousCollisionResultComparator<T>::operator()(const std::unique_ptr<ContinuousCollisionResult<T>, AlgorithmResultDeleter> &result1,
            const std::unique_ptr<ContinuousCollisionResult<T>, AlgorithmResultDeleter> &result2) const {
        return result1->getTimeToHit() < result2->getTimeToHit();
    }

    //explicit template
    template class ContinuousCollisionResult<float>;
    template class ContinuousCollisionResult<double>;

    template class ContinuousCollisionResultComparator<float>;
    template class ContinuousCollisionResultComparator<double>;

}
