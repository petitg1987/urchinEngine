#pragma once

#include "math/geometry/3d/MeshData.h"

namespace urchin {

    class MeshSimplificationService {
        public:
            MeshData mergeDuplicateVertices(const MeshData&) const;
            bool isFlatMesh(const MeshData&, float) const;
            std::vector<Point3<float>> removeCloseVertices(const std::vector<Point3<float>>&, float) const;

        private:
            std::size_t findFarthestPoint(const std::vector<Point3<float>>&, const Point3<float>&) const;
    };

}
