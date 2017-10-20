#ifndef URCHINENGINE_COLLISIONCONCAVESHAPE_H
#define URCHINENGINE_COLLISIONCONCAVESHAPE_H

#include "UrchinCommon.h"

#include "utils/math/PhysicsTransform.h"

namespace urchin
{

    class CollisionConcaveShape
    {
        public:
            virtual std::vector<Triangle3D<float>> findTrianglesInAABBox(const AABBox<float> &) const = 0;
    };

}

#endif
