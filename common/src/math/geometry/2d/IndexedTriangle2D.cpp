#include <cassert>

#include <math/geometry/2d/IndexedTriangle2D.h>

namespace urchin {

    IndexedTriangle2D::IndexedTriangle2D(const std::size_t* indices) :
            indices() {
        assert(indices[0] != indices[1] && indices[0] != indices[2] && indices[1] != indices[2]);

        this->indices[0] = indices[0];
        this->indices[1] = indices[1];
        this->indices[2] = indices[2];
    }

    IndexedTriangle2D::IndexedTriangle2D(std::size_t index1, std::size_t index2, std::size_t index3) :
            indices() {
        assert(index1 != index2 && index1 != index3 && index2 != index3);

        this->indices[0] = index1;
        this->indices[1] = index2;
        this->indices[2] = index3;
    }

    const std::array<std::size_t, 3>& IndexedTriangle2D::getIndices() const {
        return indices;
    }

    std::size_t IndexedTriangle2D::getIndex(std::size_t index) const {
        assert(index <= 2);
        return indices[index];
    }

}
