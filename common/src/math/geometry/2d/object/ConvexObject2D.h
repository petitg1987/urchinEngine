#ifndef URCHINENGINE_CONVEXOBJECT2D_H
#define URCHINENGINE_CONVEXOBJECT2D_H

#include "math/algebra/point/Point2.h"
#include "math/algebra/vector/Vector2.h"

namespace urchin {

    template<class T> class ConvexObject2D {
        public:
            virtual ~ConvexObject2D() = default;

            virtual Point2<T> getSupportPoint(const Vector2<T> &) const = 0;
    };

}

#endif
