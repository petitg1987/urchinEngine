#include <collision/narrowphase/algorithm/epa/EPATriangleData.h>

namespace urchin {

    /**
    * @param distanceToOrigin Minimum distance between the triangle and the origin
    * @param closestPointToOrigin Point on the triangle nearest to origin
    * @param barycentrics Barycentrics of closest point to origin
    */
    template<class T> EPATriangleData<T>::EPATriangleData(T distanceToOrigin, const Vector3<T>& normal, const Point3<T>& closestPointToOrigin, const std::array<T, 3>& barycentrics) :
            distanceToOrigin(distanceToOrigin),
            normal(normal),
            closestPointToOrigin(closestPointToOrigin),
            barycentrics(barycentrics) {

    }

    /**
     * @return Minimum distance between the triangle and the origin. Result can be negative or positive in function of normal orientation.
     */
    template<class T> T EPATriangleData<T>::getDistanceToOrigin() const {
        return distanceToOrigin;
    }

    /**
     * @return Normal normalized of triangle
     */
    template<class T> const Vector3<T> &EPATriangleData<T>::getNormal() const {
        return normal;
    }

    template<class T> const Point3<T> &EPATriangleData<T>::getClosestPointToOrigin() const {
        return closestPointToOrigin;
    }

    /**
     * @return Barycentric of specified index for closest point to origin
     */
    template<class T> T EPATriangleData<T>::getBarycentric(unsigned int index) const {
        return barycentrics[index];
    }

    //explicit template
    template class EPATriangleData<float>;
    template class EPATriangleData<double>;

}
