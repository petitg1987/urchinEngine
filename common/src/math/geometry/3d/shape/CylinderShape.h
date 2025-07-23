#pragma once

#include <memory>

#include "math/geometry/3d/shape/ConvexShape3D.h"

namespace urchin {

    template<class T> class CylinderShape final : public ConvexShape3D<T> {
        public:
            enum CylinderOrientation {
                CYLINDER_X,
                CYLINDER_Y,
                CYLINDER_Z
            };

            CylinderShape(T, T, CylinderOrientation);

            T getRadius() const;
            T getHeight() const;
            CylinderOrientation getCylinderOrientation() const;

            std::unique_ptr<ConvexShape3D<T>> clone() const override;
            std::unique_ptr<ConvexObject3D<T>> toConvexObject(const Transform<T>&) const override;

        private:
            T radius;
            T height;
            CylinderOrientation cylinderOrientation;
    };

}
