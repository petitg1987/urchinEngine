#include "collision/narrowphase/algorithm/epa/EPAResult.h"

namespace urchin {

    template<class T> EPAResult<T>::EPAResult(bool validResult, const Point3<T>& contactPointA, const Point3<T>& contactPointB, const Vector3<T>& normal, T depth) :
            validResult(validResult),
            contactPointA(contactPointA),
            contactPointB(contactPointB),
            normal(normal),
            depth(depth) {

    }

    template<class T> EPAResult<T> EPAResult<T>::newInvalidResult() {
        return EPAResult<T>(false, Point3<T>(), Point3<T>(), Vector3<T>(), -std::numeric_limits<T>::max());
    }

    template<class T> EPAResult<T> EPAResult<T>::newCollideResult(const Point3<T>& contactPointA, const Point3<T>& contactPointB, const Vector3<T>& normal, T depth) {
        return EPAResult<T>(true, contactPointA, contactPointB, normal, depth);
    }

    template<class T> EPAResult<T> EPAResult<T>::newNoCollideResult() {
        return EPAResult<T>(true, Point3<T>(), Point3<T>(), Vector3<T>(), -std::numeric_limits<T>::max());
    }

    template<class T> bool EPAResult<T>::isValidResult() const {
        return validResult;
    }

    template<class T> bool EPAResult<T>::isCollide() const {
        #ifdef URCHIN_DEBUG
            assert(isValidResult());
        #endif
        return depth >= 0.0;
    }

    /**
     * @return Closest point of object A in case of collision
     */
    template<class T> const Point3<T>& EPAResult<T>::getContactPointA() const {
        #ifdef URCHIN_DEBUG
            assert(isCollide());
        #endif
        return contactPointA;
    }

    /**
     * @return Closest point of object B in case of collision
     */
    template<class T> const Point3<T>& EPAResult<T>::getContactPointB() const {
        #ifdef URCHIN_DEBUG
            assert(isCollide());
        #endif
        return contactPointB;
    }

    /**
     * @return Normal normalized of collision. The normal direction is toward the object B.
     */
    template<class T> const Vector3<T>& EPAResult<T>::getNormal() const {
        #ifdef URCHIN_DEBUG
            assert(isCollide());
        #endif
        return normal;
    }

    /**
     * @return Depth of collision. The depth is always positive in case of collision.
     */
    template<class T> T EPAResult<T>::getPenetrationDepth() const {
        #ifdef URCHIN_DEBUG
            assert(isCollide());
        #endif
        return depth;
    }

    //explicit template
    template class EPAResult<float>;
    template class EPAResult<double>;

}
