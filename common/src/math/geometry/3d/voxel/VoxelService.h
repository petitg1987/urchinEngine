#pragma once

#include <vector>

#include <math/geometry/3d/voxel/VoxelGrid.h>
#include <math/algebra/point/Point3.h>
#include <math/geometry/3d/object/AABBox.h>
#include <math/geometry/3d/object/Triangle3D.h>
#include <math/geometry/3d/MeshData.h>

namespace urchin {

    class VoxelService {
        public:
            VoxelGrid voxelizeManifoldMesh(float, const MeshData&) const;

            std::vector<AABBox<float>> voxelGridToAABBoxes(const VoxelGrid&) const;

        private:
            AABBox<float> computeAABBox(const std::vector<Point3<float>>&) const;
            bool isPositionInModel(const Point3<float>&, const std::vector<Triangle3D<float>>&) const;

            void expandOnAxis(int, const VoxelGrid&, VoxelContainer&, VoxelContainer&) const;
            int getMaxInDirection(int, bool, const VoxelContainer&) const;
            AABBox<float> voxelBoxToAABBox(const VoxelContainer&, const VoxelGrid&) const;
    };

}
