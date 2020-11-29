#ifndef URCHINENGINE_BOXSHAPE_H
#define URCHINENGINE_BOXSHAPE_H

#include <vector>
#include <memory>

#include "math/geometry/3d/shape/ConvexShape3D.h"
#include "math/algebra/vector/Vector3.h"

namespace urchin {

    template<class T> class BoxShape : public ConvexShape3D<T> {
        public:
            BoxShape();
            explicit BoxShape(const Vector3<T>&);

            T getHalfSize(unsigned int) const;
            const Vector3<T> &getHalfSizes() const;
            T getMaxHalfSize() const;
            unsigned int getMaxHalfSizeIndex() const;
            T getMinHalfSize() const;
            unsigned int getMinHalfSizeIndex() const;

            Point3<T> getSupportPoint(const Vector3<T>&) const;
            T getVolume() const;

            ConvexShape3D<T> *clone() const override;
            std::unique_ptr<ConvexObject3D<T>> toConvexObject(const Transform<T>&) const override;

        private:
            Vector3<T> halfSizes; //half sizes (width, height and depth) of box
    };

}

 #endif
