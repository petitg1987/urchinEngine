#pragma once

#include <unordered_set>

#include <math/algebra/point/Point3.h>

namespace urchin {

    class VoxelGrid {
        public:
            struct VoxelHash {
                std::size_t operator()(const Point3<int>&) const;
            };

            explicit VoxelGrid(float, Point3<float>);

            const Point3<float>& getMinCenterPosition() const;
            Point3<float> computeVoxelCenterPosition(const Point3<int>&) const;
            const std::unordered_set<Point3<int>, VoxelHash>& getVoxels() const;
            float getVoxelSize() const;

            void addVoxel(const Point3<int>&);

        private:
            float voxelSize;
            Point3<float> minCenterPosition; //center position of the voxel 0, 0, 0

            std::unordered_set<Point3<int>, VoxelHash> voxels;
    };

}