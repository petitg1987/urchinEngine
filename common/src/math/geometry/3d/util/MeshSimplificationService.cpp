#include <unordered_map>
#include <unordered_set>

#include "math/geometry/3d/util/MeshSimplificationService.h"
#include "container/VectorUtil.h"

namespace urchin {

    MeshData MeshSimplificationService::simplify(const MeshData& meshData) const {
        return mergeDuplicateVertices(meshData);
    }

    MeshData MeshSimplificationService::mergeDuplicateVertices(const MeshData& mesh) const {
        std::unordered_map<Point3<float>, uint32_t, Point3<float>::Hash> pointToNewIndex;
        std::vector<uint32_t> oldToNewIndex(mesh.getVertices().size());

        std::vector<Point3<float>> newVertices;
        std::vector<std::array<uint32_t, 3>> newTrianglesIndices;
        newTrianglesIndices.reserve(mesh.getTrianglesIndices().size());

        for (std::size_t i = 0; i < mesh.getVertices().size(); ++i) {
            const Point3<float>& point = mesh.getVertices()[i];
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

        for (const std::array<uint32_t, 3>& triangleIndices : mesh.getTrianglesIndices()) {
            std::array newTriangleIndices = {oldToNewIndex[triangleIndices[0]], oldToNewIndex[triangleIndices[1]], oldToNewIndex[triangleIndices[2]]};
            bool isDegeneratedTriangle = newTriangleIndices[0] == newTriangleIndices[1] || newTriangleIndices[1] == newTriangleIndices[2] || newTriangleIndices[0] == newTriangleIndices[2];
            if (isDegeneratedTriangle) {
                continue; //the merge collapsed the triangle
            }
            newTrianglesIndices.push_back(newTriangleIndices);
        }

        return MeshData(newVertices, newTrianglesIndices);
    }

}
