#include <limits>
#include <cassert>

#include <math/geometry/3d/voxel/VoxelService.h>
#include <math/algorithm/MathFunction.h>
#include <math/geometry/3d/LineSegment3D.h>
#include <math/geometry/3d/object/Triangle3D.h>

namespace urchin {

    VoxelGrid VoxelService::voxelizeObject(float voxelSize, const std::vector<Point3<float>>& vertices, const std::vector<unsigned int>& triangleIndices) const {
        AABBox<float> abbox = computeAABBox(vertices);

        float halfVoxelSize = voxelSize / 2.0f;
        VoxelGrid voxelGrid(voxelSize, abbox.getMin().translate(Vector3(halfVoxelSize, halfVoxelSize, halfVoxelSize)));

        int xVoxelQuantity = MathFunction::ceilToInt(abbox.getHalfSize(0) * 2.0f / voxelSize);
        int yVoxelQuantity = MathFunction::ceilToInt(abbox.getHalfSize(1) * 2.0f / voxelSize);
        int zVoxelQuantity = MathFunction::ceilToInt(abbox.getHalfSize(2) * 2.0f / voxelSize);

        for (int x = 0; x < xVoxelQuantity; ++x) {
            for (int y = 0; y < yVoxelQuantity; ++y) {
                for (int z = 0; z < zVoxelQuantity; ++z) {
                    Point3 voxelIndexPosition(x, y, z);

                    Point3 voxelCenterPosition = voxelGrid.computeVoxelCenterPosition(voxelIndexPosition);
                    if (isPositionInModel(voxelCenterPosition, vertices, triangleIndices)) {
                        voxelGrid.addVoxel(voxelIndexPosition);
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

    bool VoxelService::isPositionInModel(const Point3<float>& position, const std::vector<Point3<float>>& vertices, const std::vector<unsigned int>& triangleIndices) const {
        Vector3 arbitraryAxis(1000.0f, 0.02f, 0.0f);
        Ray ray(position, position.translate(arbitraryAxis));

        int triangleIntersectionCount = 0;
        assert(triangleIndices.size() % 3 == 0);
        for (std::size_t triIndices = 0; triIndices < triangleIndices.size(); triIndices += 3) {
            const Point3<float>& p1 = vertices[triangleIndices[triIndices + 0]];
            const Point3<float>& p2 = vertices[triangleIndices[triIndices + 1]];
            const Point3<float>& p3 = vertices[triangleIndices[triIndices + 2]];

            bool hasIntersection = Triangle3D(p1, p2, p3).collideWithRay(ray);
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

        Point3 halfVoxelSize(voxelGrid.getVoxelSize() / 2.0f, voxelGrid.getVoxelSize() / 2.0f, voxelGrid.getVoxelSize() / 2.0f);
        Point3<float> min = voxelGrid.computeVoxelCenterPosition(minVoxel) - halfVoxelSize;
        Point3<float> max = voxelGrid.computeVoxelCenterPosition(maxVoxel) + halfVoxelSize;
        return AABBox(min, max);
    }

}
