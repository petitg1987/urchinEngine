#ifndef URCHINENGINE_CONESHAPE_H
#define URCHINENGINE_CONESHAPE_H

#include <memory>

#include "math/geometry/3d/shape/ConvexShape3D.h"

namespace urchin {

    template<class T> class ConeShape : public ConvexShape3D<T> {
        public:
            enum ConeOrientation { //define direction of top
                CONE_X_POSITIVE,
                CONE_X_NEGATIVE,

                CONE_Y_POSITIVE,
                CONE_Y_NEGATIVE,

                CONE_Z_POSITIVE,
                CONE_Z_NEGATIVE
            };

            ConeShape(T, T, ConeOrientation);

            T getRadius() const;
            T getHeight() const;
            ConeOrientation getConeOrientation() const;

            ConvexShape3D<T>* clone() const override;
            std::unique_ptr<ConvexObject3D<T>> toConvexObject(const Transform<T>&) const override;

        private:
            T radius;
            T height;
            ConeOrientation coneOrientation;
    };

}

#endif
