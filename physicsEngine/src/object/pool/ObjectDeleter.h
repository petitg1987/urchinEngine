#pragma once

namespace urchin {

    class CollisionConvexObject3D;

    class ObjectDeleter {
        public:
            void operator()(CollisionConvexObject3D*) const;
    };

}
