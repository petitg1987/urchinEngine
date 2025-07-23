#pragma once

#include <UrchinCommon.h>

#include "shape/CollisionTriangleShape.h"

namespace urchin {

    class CollisionConcaveShape {
        public:
            virtual ~CollisionConcaveShape() = default;

            virtual const std::vector<CollisionTriangleShape>& findTrianglesInAABBox(const AABBox<float>&) const = 0;
            virtual const std::vector<CollisionTriangleShape>& findTrianglesHitByRay(const LineSegment3D<float>&) const = 0;
    };

}
