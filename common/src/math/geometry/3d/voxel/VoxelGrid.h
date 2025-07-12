#pragma once

#include <unordered_set>

#include <math/algebra/point/Point3.h>

namespace urchin {

    struct VoxelHash {
        std::size_t operator()(const Point3<int>&) const;
    };
    using VoxelContainer = std::unordered_set<Point3<int>, VoxelHash>;

    class VoxelGrid {
        public:
            explicit VoxelGrid(Vector3<float>, Point3<float>);

            const Point3<float>& getMinCenterPosition() const;
            Point3<float> computeVoxelCenterPosition(const Point3<int>&) const;
            const VoxelContainer& getVoxels() const;
            const Vector3<float>& getVoxelSize() const;

            void addVoxel(const Point3<int>&);

        private:
            Vector3<float> voxelSize;
            Point3<float> minCenterPosition; //center position of the voxel 0, 0, 0

            VoxelContainer voxels;
    };

}