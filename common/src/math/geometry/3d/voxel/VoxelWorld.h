#pragma once

#include <unordered_map>

#include <math/geometry/3d/voxel/Voxel.h>
#include <math/algebra/point/Point3.h>

namespace urchin {

    class VoxelWorld {
        public:
            struct VoxelPositionHash {
                std::size_t operator()(const Point3<int>&) const;
            };

            explicit VoxelWorld(Point3<float>);

        private:
            Point3<float> startPosition;
            std::unordered_map<Point3<int>, Voxel, VoxelPositionHash> voxels;
    };

}