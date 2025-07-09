#pragma once

#include <unordered_set>

#include <math/algebra/point/Point3.h>

namespace urchin {

    class VoxelGrid {
        public:
            struct VoxelPositionHash {
                std::size_t operator()(const Point3<int>&) const;
            };

            explicit VoxelGrid(float, Point3<float>);

        private:
            float voxelSize;
            Point3<float> startPosition;

            std::unordered_set<Point3<int>, VoxelPositionHash> voxels;
    };

}