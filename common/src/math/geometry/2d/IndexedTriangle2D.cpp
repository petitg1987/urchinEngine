#include <cassert>

#include <math/geometry/2d/IndexedTriangle2D.h>

namespace urchin {

    template<class T> IndexedTriangle2D<T>::IndexedTriangle2D(const std::size_t* indices) :
            indices() {
        assert(indices[0] != indices[1] && indices[0] != indices[2] && indices[1] != indices[2]);

        this->indices[0] = indices[0];
        this->indices[1] = indices[1];
        this->indices[2] = indices[2];
    }

    template<class T> IndexedTriangle2D<T>::IndexedTriangle2D(std::size_t index1, std::size_t index2, std::size_t index3) :
            indices() {
        assert(index1 != index2 && index1 != index3 && index2 != index3);

        this->indices[0] = index1;
        this->indices[1] = index2;
        this->indices[2] = index3;
    }

    template<class T> const std::size_t *IndexedTriangle2D<T>::getIndices() const {
        return indices;
    }

    template<class T> std::size_t IndexedTriangle2D<T>::getIndex(std::size_t index) const {
        assert(index <= 2);

        return indices[index];
    }

    //explicit template
    template class IndexedTriangle2D<float>;

    template class IndexedTriangle2D<double>;

}
