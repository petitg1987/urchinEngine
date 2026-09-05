#include <cmath>
#include <unordered_map>
#include <unordered_set>

#include "math/geometry/3d/util/MeshSimplificationService.h"
#include "math/geometry/3d/Plane.h"
#include "container/VectorUtil.h"
#include "util/HashUtil.h"

namespace urchin {

    MeshData MeshSimplificationService::mergeDuplicateVertices(const MeshData& meshData) {
        std::unordered_map<Point3<float>, uint32_t, Point3<float>::Hash> pointToNewIndex;
        std::vector<uint32_t> oldToNewIndex(meshData.getVertices().size());

        std::vector<Point3<float>> newVertices;
        std::vector<std::array<uint32_t, 3>> newTrianglesIndices;
        newTrianglesIndices.reserve(meshData.getTrianglesIndices().size());

        for (std::size_t i = 0; i < meshData.getVertices().size(); ++i) {
            const Point3<float>& point = meshData.getVertices()[i];
            auto itFind = pointToNewIndex.find(point);
            if (itFind == pointToNewIndex.end()) {
                uint32_t newIndex = (uint32_t)newVertices.size();
                pointToNewIndex[point] = newIndex;
                newVertices.push_back(point);
                oldToNewIndex[i] = newIndex;
            } else {
                oldToNewIndex[i] = itFind->second;
            }
        }

        for (const std::array<uint32_t, 3>& triangleIndices : meshData.getTrianglesIndices()) {
            std::array newTriangleIndices = {oldToNewIndex[triangleIndices[0]], oldToNewIndex[triangleIndices[1]], oldToNewIndex[triangleIndices[2]]};
            bool isDegeneratedTriangle = newTriangleIndices[0] == newTriangleIndices[1] || newTriangleIndices[1] == newTriangleIndices[2] || newTriangleIndices[0] == newTriangleIndices[2];
            if (isDegeneratedTriangle) {
                continue; //the merge collapsed the triangle
            }
            newTrianglesIndices.push_back(newTriangleIndices);
        }

        return MeshData(newVertices, newTrianglesIndices);
    }

    bool MeshSimplificationService::isFlatMesh(const MeshData& meshData, float planeDistanceThreshold) {
        const std::vector<Point3<float>>& vertices = meshData.getVertices();
        if (vertices.size() < 4) {
            return true;
        }

        Point3<float> firstPoint = vertices[0];
        Vector3<float> firstAxis = firstPoint.vector(vertices[findFarthestPoint(vertices, firstPoint)]);
        if (firstAxis.length() < planeDistanceThreshold) {
            return true;
        }
        firstAxis = firstAxis.normalize();

        Vector3 planeNormal(0.0f, 0.0f, 0.0f);
        float maxDistanceToAxis = 0.0f;
        for (std::size_t i = 1; i < vertices.size(); ++i) {
            Vector3<float> normalCandidate = firstAxis.crossProduct(firstPoint.vector(vertices[i]));
            float distanceToAxis = normalCandidate.length();
            if (distanceToAxis > maxDistanceToAxis) {
                maxDistanceToAxis = distanceToAxis;
                planeNormal = normalCandidate;
            }
        }
        if (maxDistanceToAxis < planeDistanceThreshold) {
            return true;
        }

        Plane plane(planeNormal.normalize(), firstPoint);
        for (const Point3<float>& vertex : vertices) {
            if (std::abs(plane.distance(vertex)) > planeDistanceThreshold) {
                return false;
            }
        }

        return true;
    }

    std::vector<Point3<float>> MeshSimplificationService::downsampleVertices(const std::vector<Point3<float>>& vertices, float minDistance) {
        struct GridCellHash {
            std::size_t operator()(const std::array<int, 3>& gridCell) const {
                std::size_t seed = 0;
                HashUtil::hashCombine(seed, gridCell[0], gridCell[1], gridCell[2]);
                return seed;
            }
        };

        std::vector<Point3<float>> simplifiedVertices;
        simplifiedVertices.reserve(vertices.size());

        float minSquareDistance = minDistance * minDistance;
        std::unordered_map<std::array<int, 3>, std::vector<Point3<float>>, GridCellHash> keptVerticesByGridCell;

        for (const Point3<float>& vertex : vertices) {
            std::array gridCell = {(int)std::floor(vertex.X / minDistance), (int)std::floor(vertex.Y / minDistance), (int)std::floor(vertex.Z / minDistance)};

            bool hasCloseVertex = false;
            for (int xOffset = -1; xOffset <= 1 && !hasCloseVertex; ++xOffset) {
                for (int yOffset = -1; yOffset <= 1 && !hasCloseVertex; ++yOffset) {
                    for (int zOffset = -1; zOffset <= 1 && !hasCloseVertex; ++zOffset) {
                        auto itFind = keptVerticesByGridCell.find({gridCell[0] + xOffset, gridCell[1] + yOffset, gridCell[2] + zOffset});
                        if (itFind == keptVerticesByGridCell.end()) {
                            continue;
                        }

                        for (const Point3<float>& keptVertex : itFind->second) {
                            if (vertex.squareDistance(keptVertex) < minSquareDistance) {
                                hasCloseVertex = true;
                                break;
                            }
                        }
                    }
                }
            }

            if (!hasCloseVertex) {
                simplifiedVertices.push_back(vertex);
                keptVerticesByGridCell[gridCell].push_back(vertex);
            }
        }

        return simplifiedVertices;
    }

    std::size_t MeshSimplificationService::findFarthestPoint(const std::vector<Point3<float>>& points, const Point3<float>& refPoint) {
        std::size_t farthestPointIndex = 0;

        float maxDistance = 0;
        for (std::size_t i = 0; i < points.size(); ++i) {
            float distance = refPoint.squareDistance(points[i]);
            if (distance > maxDistance) {
                maxDistance = distance;
                farthestPointIndex = i;
            }
        }

        return farthestPointIndex;
    }

}
