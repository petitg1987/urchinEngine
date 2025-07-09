#include <limits>

#include <math/geometry/3d/voxel/VoxelService.h>

#include "math/algorithm/MathFunction.h"

namespace urchin {

    VoxelGrid VoxelService::voxelize(float voxelSize, const std::vector<Point3<float>>& vertices, const std::vector<unsigned int>& triangleIndices) const {
        AABBox<float> abbox = computeAABBox(vertices);

        VoxelGrid voxelGrid(voxelSize, abbox.getMin());

        float xSize = abbox.getHalfSize(0) * 2.0f + voxelSize /* half voxel size on each side */;
        float ySize = abbox.getHalfSize(1) * 2.0f + voxelSize /* half voxel size on each side */;
        float zSize = abbox.getHalfSize(2) * 2.0f + voxelSize /* half voxel size on each side */;
        int xVoxelQuantity = MathFunction::ceilToInt(xSize / voxelSize);
        int yVoxelQuantity = MathFunction::ceilToInt(ySize / voxelSize);
        int zVoxelQuantity = MathFunction::ceilToInt(zSize / voxelSize);

        for (int x = 0; x < xVoxelQuantity; ++x) {
            for (int y = 0; y < yVoxelQuantity; ++y) {
                for (int z = 0; z < zVoxelQuantity; ++z) {
                    Point3 voxelCenterPosition = voxelGrid.getMinCenterPosition().translate(Vector3(x * voxelSize, y * voxelSize, z * voxelSize));
                    if (isVoxelExist(voxelCenterPosition, vertices, triangleIndices)) {
                        voxelGrid.addVoxel(Point3(x, y, z));
                    }
                }
            }
        }

        return voxelGrid;
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

    bool VoxelService::isVoxelExist(const Point3<float>& /*voxelCenterPosition*/, const std::vector<Point3<float>>& /*vertices*/, const std::vector<unsigned int>& /*triangleIndices*/) const {
        return true; //TODO impl...
    }

}
