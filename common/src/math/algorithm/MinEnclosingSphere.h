#pragma once

#include <span>

#include "math/geometry/3d/object/Sphere.h"

namespace urchin {

    template<class T> class MinEnclosingSphere {
        public:
            Sphere<T> compute(std::span<Point3<T>>);
            template<std::size_t ARRAY_SIZE> Sphere<T> compute(const std::array<Point3<T>, ARRAY_SIZE>&);

        private:
            static constexpr unsigned int DIMENSION = 3;
    };

}
