#ifndef URCHINENGINE_CAPSULESHAPE_H
#define URCHINENGINE_CAPSULESHAPE_H

#include <memory>

#include "math/geometry/3d/shape/ConvexShape3D.h"

namespace urchin {

    template<class T> class CapsuleShape : public ConvexShape3D<T> {
        public:
            enum CapsuleOrientation
            {
                CAPSULE_X,
                CAPSULE_Y,
                CAPSULE_Z
            };

            CapsuleShape(T, T, CapsuleOrientation);

            T getRadius() const;
            T getCylinderHeight() const;
            CapsuleOrientation getCapsuleOrientation() const;

            T computeHeight() const;

            ConvexShape3D<T> *clone() const override;
            std::unique_ptr<ConvexObject3D<T>> toConvexObject(const Transform<T> &) const override;

        private:
            T radius;
            T cylinderHeight;
            CapsuleOrientation capsuleOrientation;
    };

}

 #endif
