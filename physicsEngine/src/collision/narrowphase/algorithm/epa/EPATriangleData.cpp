#include <collision/narrowphase/algorithm/epa/EPATriangleData.h>

namespace urchin {

    /**
    * @param distanceToOrigin Minimum distance between the triangle and the origin
    * @param closestPointToOrigin Point on the triangle nearest to origin
    * @param barycentrics Barycentrics of closest point to origin
    */
    template<class T> EPATriangleData<T>::EPATriangleData(T distanceToOrigin, const Vector3<T>& normal, const Point3<T>& closestPointToOrigin, T barycentrics[3]) :
            distanceToOrigin(distanceToOrigin),
            normal(normal),
            closestPointToOrigin(closestPointToOrigin) {
        for (unsigned int i = 0; i < 3; ++i) {
            this->barycentrics[i] = barycentrics[i];
        }
    }

    /**
     * @param Minimum distance between the triangle and the origin. Result can be negative or positive in function of normal orientation.
     */
    template<class T> T EPATriangleData<T>::getDistanceToOrigin() const {
        return distanceToOrigin;
    }

    /**
     * @param Normal normalized of triangle
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
