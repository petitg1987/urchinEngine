#pragma once

#include <vector>
#include <array>

#include "math/algebra/point/Point3.h"
#include "math/algebra/vector/Vector3.h"

namespace urchin {

    template<class T> class IndexedTriangle3D {
        public:
            explicit IndexedTriangle3D(const std::size_t*);
            IndexedTriangle3D(std::size_t, std::size_t, std::size_t);

            const std::array<std::size_t, 3>& getIndices() const;
            std::size_t getIndex(std::size_t) const;
            Vector3<T> computeNormal(const std::vector<Point3<T>>&) const;
            Vector3<T> computeNormal(const Point3<T>&, const Point3<T>&, const Point3<T>&) const;

        private:
            std::array<std::size_t, 3> indices;
    };

}
