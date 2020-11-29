#ifndef URCHINENGINE_RECTANGLESHAPE_H
#define URCHINENGINE_RECTANGLESHAPE_H

#include "math/geometry/2d/shape/ConvexShape2D.h"
#include "math/algebra/vector/Vector2.h"

namespace urchin {

    /**
    * Represents a rectangle aligned on axis
    */
    template<class T> class RectangleShape : public ConvexShape2D {
        public:
            explicit RectangleShape(const Vector2<T>&);

            const Vector2<T> &getDiagonal() const;

        private:
            Vector2<T> diagonal; //diagonal from minimum to maximum point
    };

}

#endif
