#pragma once

#include <vector>

#include <math/geometry/3d/voxel/VoxelWorld.h>
#include <math/algebra/point/Point3.h>
#include <math/geometry/3d/object/AABBox.h>

namespace urchin {

    class VoxelService {
        public:
            VoxelWorld voxelize(const std::vector<Point3<float>>&, const std::vector<unsigned int>&) const;

        private:
            AABBox<float> computeAABBox(const std::vector<Point3<float>>&) const;

    };

}
