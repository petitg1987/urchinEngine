#include <limits>

#include <math/geometry/3d/voxel/VoxelService.h>
#include <math/algorithm/MathFunction.h>

namespace urchin {

    VoxelGrid VoxelService::voxelizeManifoldMesh(float expectedVoxelSize, const MeshData& mesh) const {
        std::vector<Triangle3D<float>> triangles;
        triangles.reserve(mesh.getTrianglesIndices().size());
        for (const std::array<uint32_t, 3>& triangleIndices : mesh.getTrianglesIndices()) {
            triangles.emplace_back(mesh.getVertices()[triangleIndices[0]], mesh.getVertices()[triangleIndices[1]], mesh.getVertices()[triangleIndices[2]]);
        }

        AABBox<float> boundingBox = computeAABBox(mesh.getVertices());
        int xVoxelQuantity = MathFunction::ceilToInt(boundingBox.getHalfSize(0) * 2.0f / expectedVoxelSize);
        int yVoxelQuantity = MathFunction::ceilToInt(boundingBox.getHalfSize(1) * 2.0f / expectedVoxelSize);
        int zVoxelQuantity = MathFunction::ceilToInt(boundingBox.getHalfSize(2) * 2.0f / expectedVoxelSize);

        Vector3 voxelSize(
            boundingBox.getHalfSize(0) * 2.0f / (float)xVoxelQuantity,
            boundingBox.getHalfSize(1) * 2.0f / (float)yVoxelQuantity,
            boundingBox.getHalfSize(2) * 2.0f / (float)zVoxelQuantity
        );

        constexpr float EPSILON = 0.0001f; //to avoid to be on the edge of the bounding box and miss the collision
        std::array shiftTestPoints = {
            Vector3(0.0f, 0.0f, 0.0f), //central point
            Vector3(voxelSize.X / 2.0f - EPSILON, 0.0f, 0.0f), //X+
            Vector3(-voxelSize.X / 2.0f + EPSILON, 0.0f, 0.0f), //X-
            Vector3(0.0f, voxelSize.Y / 2.0f - EPSILON, 0.0f), //Y+
            Vector3(0.0f, -voxelSize.Y / 2.0f + EPSILON, 0.0f), //Y-
            Vector3(0.0f, 0.0f, voxelSize.Z / 2.0f - EPSILON), //Z+
            Vector3(0.0f, 0.0f, -voxelSize.Z / 2.0f + EPSILON), //Z-
        };

        VoxelGrid voxelGrid(voxelSize, boundingBox.getMin().translate(voxelSize / 2.0f));

        for (int x = 0; x < xVoxelQuantity; ++x) {
            for (int y = 0; y < yVoxelQuantity; ++y) {
                for (int z = 0; z < zVoxelQuantity; ++z) {
                    Point3 voxelIndexPosition(x, y, z);
                    Point3 voxelCenterPosition = voxelGrid.computeVoxelCenterPosition(voxelIndexPosition);

                    for (const Vector3<float>& shiftTestPoint : shiftTestPoints) {
                        Point3 testPoint = voxelCenterPosition.translate(shiftTestPoint);
                        if (isPositionInModel(testPoint, triangles)) {
                            voxelGrid.addVoxel(voxelIndexPosition);
                            break;
                        }
                    }
                }
            }
        }

        return voxelGrid;
    }

    AABBox<float> VoxelService::computeAABBox(const std::vector<Point3<float>>& vertices) const {
        Point3 min(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
        Point3 max(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
        for (const Point3<float>& vertex : vertices) {
            min.X = std::min(min.X, vertex.X);
            min.Y = std::min(min.Y, vertex.Y);
            min.Z = std::min(min.Z, vertex.Z);

            max.X = std::max(max.X, vertex.X);
            max.Y = std::max(max.Y, vertex.Y);
            max.Z = std::max(max.Z, vertex.Z);
        }
        return AABBox(min, max);
    }

    bool VoxelService::isPositionInModel(const Point3<float>& position, const std::vector<Triangle3D<float>>& triangles) const {
        Vector3 arbitraryAxis(1000.0f, 1000.1f, 1000.2f);
        Ray ray(position, position.translate(arbitraryAxis));

        int triangleIntersectionCount = 0;
        for (const Triangle3D<float>& triangle : triangles) {
            bool hasIntersection = triangle.collideWithRay(ray);
            if (hasIntersection) {
                triangleIntersectionCount++;
            }
        }

        return triangleIntersectionCount % 2 == 1;
    }

    std::vector<AABBox<float>> VoxelService::voxelGridToAABBoxes(const VoxelGrid& voxelGrid) const {
        std::vector<AABBox<float>> result;

        VoxelContainer usedVoxels;
        for (Point3 voxelIndexPosition : voxelGrid.getVoxels()) {
            if (usedVoxels.contains(voxelIndexPosition)) {
                continue;
            }

            VoxelContainer voxelBox;
            voxelBox.insert(voxelIndexPosition);
            usedVoxels.insert(voxelIndexPosition);
            for (int axis = 0; axis < 3; ++axis) {
                expandOnAxis(axis, voxelGrid, voxelBox, usedVoxels);
            }

            result.push_back(voxelBoxToAABBox(voxelBox, voxelGrid));
        }

        return result;
    }

    void VoxelService::expandOnAxis(int axis, const VoxelGrid& voxelGrid, VoxelContainer& voxelBox, VoxelContainer& usedVoxels) const {
        int otherAxisMin1 = getMaxInDirection((axis + 1) % 3, false, voxelBox);
        int otherAxisMax1 = getMaxInDirection((axis + 1) % 3, true, voxelBox);
        int otherAxisMin2 = getMaxInDirection((axis + 2) % 3, false, voxelBox);
        int otherAxisMax2 = getMaxInDirection((axis + 2) % 3, true, voxelBox);

        int totalVoxelsToAdd = (otherAxisMax1 - otherAxisMin1 + 1) * (otherAxisMax2 - otherAxisMin2);
        std::vector<Point3<int>> voxelsToAdd;
        voxelsToAdd.reserve(totalVoxelsToAdd);

        for (bool isPositive : {true, false}) { //expand on 'axis' in both direction: positive and negative
            bool allVoxelsCanBeAdded = true;
            do { //expand in the current direction until it cannot expand anymore
                voxelsToAdd.clear();
                allVoxelsCanBeAdded = true;

                Point3 voxelToAdd(0, 0, 0);
                voxelToAdd[axis] = getMaxInDirection(axis, isPositive, voxelBox) + (isPositive ? 1 : -1);
                for (int i = otherAxisMin1; i <= otherAxisMax1 && allVoxelsCanBeAdded; ++i) {
                    voxelToAdd[(axis + 1) % 3] = i;
                    for (int j = otherAxisMin2; j <= otherAxisMax2; ++j) {
                        voxelToAdd[(axis + 2) % 3] = j;

                        if (!voxelGrid.getVoxels().contains(voxelToAdd) || usedVoxels.contains(voxelToAdd)) {
                            allVoxelsCanBeAdded = false;
                            break;
                        }
                        voxelsToAdd.push_back(voxelToAdd);
                    }
                }

                if (allVoxelsCanBeAdded) {
                    for (const Point3<int>& voxel : voxelsToAdd) {
                        voxelBox.insert(voxel);
                        usedVoxels.insert(voxel);
                    }
                }
            } while (allVoxelsCanBeAdded && !voxelsToAdd.empty());
        }
    }

    int VoxelService::getMaxInDirection(int axis, bool isPositive, const VoxelContainer& voxelBox) const {
        Point3<int> startVoxel = *voxelBox.begin();
        int currPosition = startVoxel[axis];
        while (true) {
            currPosition += isPositive ? 1 : -1;

            Point3<int> voxelPosition = startVoxel;
            voxelPosition[axis] = currPosition;
            if (!voxelBox.contains(voxelPosition)) {
                return currPosition - (isPositive ? 1 : -1);
            }
        }
    }

    AABBox<float> VoxelService::voxelBoxToAABBox(const VoxelContainer& voxelBox, const VoxelGrid& voxelGrid) const {
        Point3 minVoxel(0, 0, 0);
        Point3 maxVoxel(0, 0, 0);
        for (int axis = 0; axis < 3; ++axis) {
            minVoxel[axis] = getMaxInDirection(axis, false, voxelBox);
            maxVoxel[axis] = getMaxInDirection(axis, true, voxelBox);
        }

        Point3<float> min = voxelGrid.computeVoxelCenterPosition(minVoxel).translate(-voxelGrid.getVoxelSize() / 2.0f);
        Point3<float> max = voxelGrid.computeVoxelCenterPosition(maxVoxel).translate(voxelGrid.getVoxelSize() / 2.0f);
        return AABBox(min, max);
    }

}
