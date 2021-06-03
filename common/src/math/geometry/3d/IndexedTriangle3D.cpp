#include <cassert>

#include <math/geometry/3d/IndexedTriangle3D.h>

namespace urchin {

    template<class T> IndexedTriangle3D<T>::IndexedTriangle3D(const std::size_t* indices) :
            indices() {
        assert(indices[0] != indices[1] && indices[0] != indices[2] && indices[1] != indices[2]);

        this->indices[0] = indices[0];
        this->indices[1] = indices[1];
        this->indices[2] = indices[2];
    }

    template<class T> IndexedTriangle3D<T>::IndexedTriangle3D(std::size_t index1, std::size_t index2, std::size_t index3) :
            indices() {
        assert(index1 != index2 && index1 != index3 && index2 != index3);

        this->indices[0] = index1;
        this->indices[1] = index2;
        this->indices[2] = index3;
    }

    template<class T> const std::size_t *IndexedTriangle3D<T>::getIndices() const {
        return indices;
    }

    template<class T> std::size_t IndexedTriangle3D<T>::getIndex(std::size_t index) const {
        assert(index <= 2);

        return indices[index];
    }

    template<class T> Vector3<T> IndexedTriangle3D<T>::computeNormal(const std::vector<Point3<T>>& trianglePoints) const {
        const Vector3<T>& aux = trianglePoints[indices[1]].vector(trianglePoints[indices[2]]);
        return aux.crossProduct(trianglePoints[indices[1]].vector(trianglePoints[indices[0]])).normalize();
    }

    template<class T> Vector3<T> IndexedTriangle3D<T>::computeNormal(const Point3<T>& point0, const Point3<T>& point1, const Point3<T>& point2) const {
        const Vector3<T>& aux = point1.vector(point2);
        return aux.crossProduct(point1.vector(point0)).normalize();
    }

    //explicit template
    template class IndexedTriangle3D<float>;
    template class IndexedTriangle3D<double>;

}
