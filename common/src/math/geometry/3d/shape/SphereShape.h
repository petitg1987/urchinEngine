#pragma once

#include <memory>

#include "math/geometry/3d/shape/ConvexShape3D.h"

namespace urchin {

    template<class T> class SphereShape final : public ConvexShape3D<T> {
        public:
            explicit SphereShape(T);

            T getRadius() const;

            std::unique_ptr<ConvexShape3D<T>> clone() const override;
            std::unique_ptr<ConvexObject3D<T>> toConvexObject(const Transform<T>&) const override;

        private:
            T radius;
    };

}
