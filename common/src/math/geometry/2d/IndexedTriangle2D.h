#pragma once

#include <vector>
#include <map>
#include <array>

#include <math/algebra/point/Point2.h>
#include <math/algebra/vector/Vector2.h>

namespace urchin {

    template<class T> class IndexedTriangle2D {
        public:
            explicit IndexedTriangle2D(const std::size_t*);
            IndexedTriangle2D(std::size_t, std::size_t, std::size_t);

            const std::array<std::size_t, 3>& getIndices() const;
            std::size_t getIndex(std::size_t) const;

        private:
            std::array<std::size_t, 3> indices;
    };

}
