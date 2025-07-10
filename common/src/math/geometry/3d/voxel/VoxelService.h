#pragma once

#include <vector>
#include <unordered_set>

#include <math/geometry/3d/voxel/VoxelGrid.h>
#include <math/algebra/point/Point3.h>
#include <math/geometry/3d/object/AABBox.h>

namespace urchin {

    class VoxelService {
        public:
            using VoxelContainer = std::unordered_set<Point3<int>, VoxelGrid::VoxelHash>;

            VoxelGrid voxelizeObject(float, const std::vector<Point3<float>>&, const std::vector<unsigned int>&) const;

            std::vector<AABBox<float>> voxelGridToAABBoxes(const VoxelGrid&) const;

        private:
            AABBox<float> computeAABBox(const std::vector<Point3<float>>&) const;
            bool isVoxelExist(const Point3<float>&, const std::vector<Point3<float>>&, const std::vector<unsigned int>&) const;

            void expand(int, bool, const VoxelGrid&, VoxelContainer&, VoxelContainer&) const;
            int getMaxInDirection(int, bool, const VoxelContainer&) const;
            AABBox<float> voxelBoxToAABBox(const VoxelContainer&) const;
    };

}
