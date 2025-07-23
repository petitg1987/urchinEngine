#include "math/geometry/3d/voxel/VoxelGrid.h"

namespace urchin {

    std::size_t VoxelHash::operator()(const Point3<int>& indexPosition) const {
        constexpr auto BIT_IN_SIZE_T = (sizeof(std::size_t) * 8) / 3;
        return ((std::size_t)indexPosition.X) +
                (((std::size_t)indexPosition.Y) << BIT_IN_SIZE_T) +
                (((std::size_t)indexPosition.Z) << BIT_IN_SIZE_T * 2);
    }

    VoxelGrid::VoxelGrid(Vector3<float> voxelSize, Point3<float> minCenterPosition) :
            voxelSize(voxelSize),
            minCenterPosition(std::move(minCenterPosition)) {

    }

    const Point3<float>& VoxelGrid::getMinCenterPosition() const {
        return minCenterPosition;
    }

    Point3<float> VoxelGrid::computeVoxelCenterPosition(const Point3<int>& voxelIndexPosition) const {
        return minCenterPosition.translate(Vector3((float)voxelIndexPosition.X * voxelSize[0], (float)voxelIndexPosition.Y * voxelSize[1], (float)voxelIndexPosition.Z * voxelSize[2]));
    }

    const VoxelContainer& VoxelGrid::getVoxels() const {
        return voxels;
    }

    const Vector3<float>& VoxelGrid::getVoxelSize() const {
        return voxelSize;
    }

    void VoxelGrid::addVoxel(const Point3<int>& voxelIndexPosition) {
        voxels.insert(voxelIndexPosition);
    }

}