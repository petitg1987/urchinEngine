#pragma once

#include "math/geometry/3d/MeshData.h"

namespace urchin {

    class MeshDataService {
        public:
            MeshDataService() = delete;

            static MeshData mergeDuplicateVertices(const MeshData&);
            static bool isFlatMesh(const MeshData&, float);
            static std::vector<Point3<float>> downsampleVertices(const std::vector<Point3<float>>&, float);
            static std::size_t findFarthestPoint(const std::vector<Point3<float>>&, const Point3<float>&);
    };

}
