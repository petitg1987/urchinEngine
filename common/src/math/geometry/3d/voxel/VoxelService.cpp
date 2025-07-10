#include <limits>
#include <cassert>

#include <math/geometry/3d/voxel/VoxelService.h>
#include <math/algorithm/MathFunction.h>
#include <math/geometry/3d/object/LineSegment3D.h>
#include <math/geometry/3d/object/Triangle3D.h>
#include <math/geometry/3d/Plane.h>

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
                    Point3 voxelCenterPosition = voxelGrid.computeVoxelPosition(voxelIndexPosition);
                    if (isVoxelExist(voxelCenterPosition, vertices, triangleIndices)) {
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

    bool VoxelService::isVoxelExist(const Point3<float>& voxelCenterPosition, const std::vector<Point3<float>>& vertices, const std::vector<unsigned int>& triangleIndices) const {
        Vector3 arbitraryAxis(1000.0f, 0.02f, 0.0f);

        int triangleIntersectionCount = 0;
        assert(triangleIndices.size() % 3 == 0);
        for (std::size_t triIndices = 0; triIndices < triangleIndices.size(); triIndices += 3) {
            const Point3<float>& p1 = vertices[triangleIndices[triIndices + 0]];
            const Point3<float>& p2 = vertices[triangleIndices[triIndices + 1]];
            const Point3<float>& p3 = vertices[triangleIndices[triIndices + 2]];

            LineSegment3D line(voxelCenterPosition, voxelCenterPosition.translate(arbitraryAxis));

            bool hasPlaneInteraction = false;
            Point3 intersectionPoint = Plane(p1, p2, p3).intersectPoint(line, hasPlaneInteraction);
            if (hasPlaneInteraction) {
                bool hasTriangleIntersection = Triangle3D(p1, p2, p3).projectedPointInsideTriangle(intersectionPoint);
                if (hasTriangleIntersection) {
                    triangleIntersectionCount++;
                }
            }
        }

        return triangleIntersectionCount % 2 == 1;
    }

    std::vector<AABBox<float>> VoxelService::voxelGridToAABBoxes(const VoxelGrid& voxelGrid) const {
        std::vector<AABBox<float>> result;

        std::array expandDirections = {
            std::pair(0, true), std::pair(0, false),
            std::pair(1, true), std::pair(1, false),
            std::pair(2, true), std::pair(2, false)
        };

        VoxelContainer usedVoxels;
        for (Point3 voxelIndexPosition : voxelGrid.getVoxels()) {
            if (usedVoxels.contains(voxelIndexPosition)) {
                continue;
            }

            VoxelContainer voxelBox;
            voxelBox.insert(voxelIndexPosition);
            for (const auto& [directionAxis, isPositive] : expandDirections) {
                expand(directionAxis, isPositive, voxelGrid, voxelBox, usedVoxels);
            }

            result.push_back(voxelBoxToAABBox(voxelBox));
        }

        return result;
    }

    void VoxelService::expand(int directionAxis, bool isPositive, const VoxelGrid& voxelGrid, VoxelContainer& voxelBox, VoxelContainer& usedVoxels) const {
        std::array<std::pair<int, int>, 2> otherAxesMinMax;
        for (int i = 0; i < 2; ++i) {
            std::size_t otherAxis = (std::size_t)((directionAxis + i + 1) % 3);
            int otherAxisMin = getMaxInDirection(otherAxis, false, voxelBox);
            int otherAxisMax = getMaxInDirection(otherAxis, true, voxelBox);
            otherAxesMinMax[i] = std::pair(otherAxisMin, otherAxisMax);
        }
        int totalVoxelsToAdd = (otherAxesMinMax[0].second - otherAxesMinMax[0].first + 1) * (otherAxesMinMax[1].second - otherAxesMinMax[1].first + 1);

        std::vector<Point3<int>> voxelsToAdd;
        voxelsToAdd.reserve(totalVoxelsToAdd);
        Point3 voxelToAdd(0, 0, 0);
        voxelToAdd[directionAxis] = getMaxInDirection(directionAxis, isPositive, voxelBox) + (isPositive ? 1 : -1);
        for (int i = otherAxesMinMax[0].first; i <= otherAxesMinMax[0].second; ++i) {
            voxelToAdd[(directionAxis + 1) % 3] = i;
            for (int j = otherAxesMinMax[1].first; j <= otherAxesMinMax[1].second; ++j) {
                voxelToAdd[(directionAxis + 2) % 3] = j;
                voxelsToAdd.push_back(voxelToAdd);
            }
        }

        bool allVoxelsCanBeAdded = true;
        for (const Point3<int>& voxel : voxelsToAdd) {
            if (!voxelGrid.getVoxels().contains(voxel) && !usedVoxels.contains(voxel)) {
                allVoxelsCanBeAdded = false;
                break;
            }
        }

        if (allVoxelsCanBeAdded) {
            for (const Point3<int>& voxel : voxelsToAdd) {
                voxelBox.insert(voxel);
                usedVoxels.insert(voxel);
            }
        }
    }

    int VoxelService::getMaxInDirection(int directionAxis, bool isPositive, const VoxelContainer& voxelBox) const {
        Point3<int> startVoxel = *voxelBox.begin();
        int currPosition = startVoxel[directionAxis];
        while (true) {
            currPosition += isPositive ? 1 : -1;

            Point3<int> voxelPosition = startVoxel;
            voxelPosition[directionAxis] = currPosition;
            if (!voxelBox.contains(voxelPosition)) {
                return currPosition - (isPositive ? 1 : -1);
            }
        }
    }

    AABBox<float> VoxelService::voxelBoxToAABBox(const std::unordered_set<Point3<int>, VoxelGrid::VoxelHash>& /*voxelBox*/) const {
        //TODO ...
        return AABBox<float>();
    }

}
