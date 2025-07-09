#include <limits>

#include <math/geometry/3d/voxel/VoxelService.h>

namespace urchin {

    VoxelWorld VoxelService::voxelize(const std::vector<Point3<float>>& vertices, const std::vector<unsigned int>& /*triangleIndices*/) const {
        AABBox<float> globalAbbox = computeAABBox(vertices);

        VoxelWorld voxelWorld(globalAbbox.getMin());
        return voxelWorld;
    }

    AABBox<float> VoxelService::computeAABBox(const std::vector<Point3<float>>& vertices) const {
        Point3 min(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
        Point3 max(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
        for (const Point3<float>& vertex : vertices) {
            min.X = std::min(min.X, vertex.X);
            min.Y = std::min(min.Y, vertex.Y);
            min.Z = std::min(min.Z, vertex.Z);

            max.X = std::max(max.X, vertex.X);
            max.Y = std::max(max.Y, vertex.Y);
            max.Z = std::max(max.Z, vertex.Z);
        }
        return AABBox(min, max);
    }

}
