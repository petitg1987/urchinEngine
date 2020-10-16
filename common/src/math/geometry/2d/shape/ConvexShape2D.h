#ifndef URCHINENGINE_CONVEXSHAPE2D_H
#define URCHINENGINE_CONVEXSHAPE2D_H

namespace urchin {

    /**
    * Represents a convex shape.
    * A shape doesn't have a position in space.
    */
    class ConvexShape2D {
        public:
            virtual ~ConvexShape2D() = default;
    };

}

#endif
