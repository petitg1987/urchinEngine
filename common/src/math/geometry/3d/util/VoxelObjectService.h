#pragma once

#include <vector>

#include <math/algebra/point/Point3.h>

namespace urchin {

    class VoxelObjectService {
        public:
            void voxelize(const std::vector<Point3<float>>&, const std::vector<unsigned int>&) const;

    };

}
