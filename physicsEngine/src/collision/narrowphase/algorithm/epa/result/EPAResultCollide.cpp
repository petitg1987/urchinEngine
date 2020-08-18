#include "EPAResultCollide.h"

namespace urchin
{

    template<class T> EPAResultCollide<T>::EPAResultCollide(const Point3<T> &contactPointA, const Point3<T> &contactPointB, const Vector3<T> &normal, T depth) :
        EPAResult<T>(),
        contactPointA(contactPointA),
        contactPointB(contactPointB),
        normal(normal),
        depth(depth)
    {

    }

    template<class T> bool EPAResultCollide<T>::isValidResult() const
    {
        return true;
    }

    template<class T> bool EPAResultCollide<T>::isCollide() const
    {
        return true;
    }

    template<class T> const Point3<T> &EPAResultCollide<T>::getContactPointA() const
    {
        return contactPointA;
    }

    /**
     * @return Closest point of object B in case of collision
     */
    template<class T> const Point3<T> &EPAResultCollide<T>::getContactPointB() const
    {
        return contactPointB;
    }

    /**
     * @return Normal normalized of collision. The normal direction is toward the object B.
     */
    template<class T> const Vector3<T> &EPAResultCollide<T>::getNormal() const
    {
        return normal;
    }

    /**
     * @return Depth of collision. The depth is positive in case of collision.
     */
    template<class T> T EPAResultCollide<T>::getPenetrationDepth() const
    {
        return depth;
    }

    //explicit template
    template class EPAResultCollide<float>;
    template class EPAResultCollide<double>;

}
