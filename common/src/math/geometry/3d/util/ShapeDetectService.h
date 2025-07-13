#pragma once

#include <memory>
#include <vector>

#include <math/algebra/point/Point3.h>
#include <math/algebra/Quaternion.h>
#include <math/geometry/3d/shape/ConvexShape3D.h>
#include <math/geometry/3d/MeshData.h>

namespace urchin {

    class ShapeDetectService {
        public:
            struct Config {
                bool voxelizationEnabled;
                float voxelizationSize;
            };

            struct LocalizedShape {
                std::unique_ptr<ConvexShape3D<float>> shape;
                Point3<float> position;
                Quaternion<float> orientation;
            };

            explicit ShapeDetectService(Config config);

            std::vector<LocalizedShape> detect(const MeshData&) const;

        private:
            std::vector<MeshData> splitDistinctMeshes(const MeshData&) const;

            std::optional<LocalizedShape> tryBuildBox(const std::vector<Point3<float>>&) const;
            std::optional<LocalizedShape> tryBuildSphere(const std::vector<Point3<float>>&) const;
            std::optional<LocalizedShape> tryBuildConvexHull(const MeshData&) const;
            std::vector<LocalizedShape> buildAABBoxes(const MeshData&) const;

            bool isConvexMesh(const MeshData&) const;
            bool isManifoldMesh(const MeshData&) const;
            AABBox<float> computeAABBox(const std::vector<Point3<float>>&) const;
            std::pair<std::size_t, std::size_t> findClosestAndFarthestPoints(const std::vector<Point3<float>>&, const Point3<float>&) const;
            std::size_t findFarthestPoint(const std::vector<Point3<float>>&, const Point3<float>&) const;

            Config config;
    };

}
