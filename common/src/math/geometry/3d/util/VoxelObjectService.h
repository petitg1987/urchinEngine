#pragma once

#include <vector>

#include <math/algebra/point/Point3.h>
#include <math/geometry/3d/object/AABBox.h>

namespace urchin {

    class VoxelObjectService {
        public:
            void voxelize(const std::vector<Point3<float>>&, const std::vector<unsigned int>&) const;


        private:
            AABBox<float> computeAABBox(const std::vector<Point3<float>>&) const;
    };

}
