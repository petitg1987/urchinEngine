#pragma once

#include <memory>

#include "math/geometry/3d/object/ConvexObject3D.h"
#include "math/algebra/Transform.h"

namespace urchin {

    /**
    * Represents a convex shape.
    * A shape doesn't have a position in space.
    */
    template<class T> class ConvexShape3D {
        public:
            virtual ~ConvexShape3D() = default;

            virtual std::unique_ptr<ConvexShape3D<T>> clone() const = 0;
            virtual std::unique_ptr<ConvexObject3D<T>> toConvexObject(const Transform<T>&) const = 0;
    };

}
