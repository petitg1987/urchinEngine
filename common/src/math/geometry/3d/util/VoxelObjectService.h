#pragma once

#include <vector>
#include <unordered_map>

#include <math/algebra/point/Point3.h>
#include <math/geometry/3d/object/AABBox.h>

namespace urchin {

    class VoxelObjectService {
        public:
            struct Voxel {
                Point3<float> position;
            };
            struct VoxelPositionHash {
                std::size_t operator()(const Point3<float>&) const;
            };

            std::unordered_map<Point3<float>, Point3<float>, VoxelPositionHash> voxelize(const std::vector<Point3<float>>&, const std::vector<unsigned int>&) const;

        private:
            AABBox<float> computeAABBox(const std::vector<Point3<float>>&) const;

    };

}
