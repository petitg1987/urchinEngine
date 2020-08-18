#include "SphereShape.h"
#include "math/geometry/3d/object/Sphere.h"

namespace urchin
{

    template<class T> SphereShape<T>::SphereShape(T radius) :
        radius(radius)
    {

    }

    template<class T> T SphereShape<T>::getRadius() const
    {
        return radius;
    }

    template<class T> ConvexShape3D<T> *SphereShape<T>::clone() const
    {
        return new SphereShape<T>(*this);
    }

    template<class T> std::unique_ptr<ConvexObject3D<T>> SphereShape<T>::toConvexObject(const Transform<T> &transform) const
    {
        return std::make_unique<Sphere<T>>(radius*transform.getScale(), transform.getPosition());
    }

    //explicit template
    template class SphereShape<float>;
    template class SphereShape<double>;

}
