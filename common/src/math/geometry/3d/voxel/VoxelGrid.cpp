#include <math/geometry/3d/voxel/VoxelGrid.h>

namespace urchin {

    std::size_t VoxelHash::operator()(const Point3<int>& indexPosition) const {
        std::size_t h1 = std::hash<int>()(indexPosition.X);
        std::size_t h2 = std::hash<int>()(indexPosition.Y);
        std::size_t h3 = std::hash<int>()(indexPosition.Z);
        return h1 ^ (h2 << 1) ^ (h3 << 2); //TODO improve speed. Only shift bit without hash ?
    }

    VoxelGrid::VoxelGrid(float voxelSize, Point3<float> minCenterPosition) :
            voxelSize(voxelSize),
            minCenterPosition(std::move(minCenterPosition)) {

    }

    const Point3<float>& VoxelGrid::getMinCenterPosition() const {
        return minCenterPosition;
    }

    Point3<float> VoxelGrid::computeVoxelCenterPosition(const Point3<int>& voxelIndexPosition) const {
        return minCenterPosition.translate(Vector3((float)voxelIndexPosition.X * voxelSize, (float)voxelIndexPosition.Y * voxelSize, (float)voxelIndexPosition.Z * voxelSize));
    }

    const VoxelContainer& VoxelGrid::getVoxels() const {
        return voxels;
    }

    float VoxelGrid::getVoxelSize() const {
        return voxelSize;
    }

    void VoxelGrid::addVoxel(const Point3<int>& voxelIndexPosition) {
        voxels.insert(voxelIndexPosition);
    }

}