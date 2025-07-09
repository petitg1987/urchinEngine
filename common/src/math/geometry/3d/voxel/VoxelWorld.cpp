#include <math/geometry/3d/voxel/VoxelWorld.h>

namespace urchin {

    std::size_t VoxelWorld::VoxelPositionHash::operator()(const Point3<int>& indexPosition) const {
        std::size_t h1 = std::hash<int>()(indexPosition.X);
        std::size_t h2 = std::hash<int>()(indexPosition.Y);
        std::size_t h3 = std::hash<int>()(indexPosition.Z);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }


    VoxelWorld::VoxelWorld(Point3<float> startPosition) :
            startPosition(std::move(startPosition)) {

    }

}