#include <unordered_map>
#include <unordered_set>

#include <math/geometry/3d/util/MeshSimplificationService.h>
#include <container/VectorUtil.h>

namespace urchin {

    uint64_t MeshSimplificationService::Edge::Hash::operator()(const Edge& edge) const {
        return ((uint64_t)edge.vertexIndex1) << 32 | (uint64_t)edge.vertexIndex2;
    }

    MeshSimplificationService::MeshSimplificationService(Config config) :
            config(std::move(config)) {

    }

    MeshData MeshSimplificationService::simplify(const MeshData& meshData) const {
        MeshData meshUniqueVertices = mergeDuplicateVertices(meshData);
        return collapseShortEdge(meshUniqueVertices);
    }

    MeshData MeshSimplificationService::mergeDuplicateVertices(const MeshData& mesh) const {
        std::unordered_map<Point3<float>, unsigned int, Point3<float>::Hash> pointToNewIndex;
        std::vector<unsigned int> oldToNewIndex(mesh.getVertices().size());

        std::vector<Point3<float>> newVertices;
        std::vector<std::array<uint32_t, 3>> newTrianglesIndices;
        newTrianglesIndices.resize(mesh.getTrianglesIndices().size());

        for (std::size_t i = 0; i < mesh.getVertices().size(); ++i) {
            const Point3<float>& point = mesh.getVertices()[i];
            auto itFind = pointToNewIndex.find(point);
            if (itFind == pointToNewIndex.end()) {
                unsigned int newIndex = (unsigned int)newVertices.size();
                pointToNewIndex[point] = newIndex;
                newVertices.push_back(point);
                oldToNewIndex[i] = newIndex;
            } else {
                oldToNewIndex[i] = itFind->second;
            }
        }

        for (std::size_t triangleIndex = 0; triangleIndex < mesh.getTrianglesIndices().size(); ++triangleIndex) {
            newTrianglesIndices[triangleIndex][0] = oldToNewIndex[mesh.getTrianglesIndices()[triangleIndex][0]];
            newTrianglesIndices[triangleIndex][1] = oldToNewIndex[mesh.getTrianglesIndices()[triangleIndex][1]];
            newTrianglesIndices[triangleIndex][2] = oldToNewIndex[mesh.getTrianglesIndices()[triangleIndex][2]];
        }

        return MeshData(newVertices, newTrianglesIndices);
    }

    MeshData MeshSimplificationService::collapseShortEdge(const MeshData& mesh) const {
        std::vector<Point3<float>> newVertices = mesh.getVertices();
        std::vector<std::array<uint32_t, 3>> newTrianglesIndices = mesh.getTrianglesIndices();

        const auto buildEdge = [](uint32_t i1, uint32_t i2){ return Edge{.vertexIndex1 = i1 < i2 ? i1 : i2, .vertexIndex2 = i1 < i2 ? i2 : i1}; };
        std::unordered_map<Edge, std::vector<std::size_t>, Edge::Hash> edgeToTriangles;
        std::unordered_map<uint32_t, std::unordered_set<std::size_t>> vertexToTriangles;
        for (std::size_t triangleIndex = 0; triangleIndex < newTrianglesIndices.size(); ++triangleIndex) {
            const std::array<uint32_t, 3>& triangleIndices = newTrianglesIndices[triangleIndex];

            edgeToTriangles[buildEdge(triangleIndices[0], triangleIndices[1])].push_back(triangleIndex);
            edgeToTriangles[buildEdge(triangleIndices[1], triangleIndices[2])].push_back(triangleIndex);
            edgeToTriangles[buildEdge(triangleIndices[2], triangleIndices[0])].push_back(triangleIndex);

            vertexToTriangles[triangleIndices[0]].insert(triangleIndex);
            vertexToTriangles[triangleIndices[1]].insert(triangleIndex);
            vertexToTriangles[triangleIndices[2]].insert(triangleIndex);
        }

        std::unordered_set<std::size_t> removedTriangles;
        std::unordered_map<uint32_t, uint32_t> oldToNewEdgeIndexMap;
        float edgeSquareDistanceThreshold = config.edgeDistanceThreshold * config.edgeDistanceThreshold;
        for (auto& [edge, trianglesIndex] : edgeToTriangles) {
            uint32_t edgeVertexIndex1 = edge.vertexIndex1;
            while (oldToNewEdgeIndexMap.contains(edgeVertexIndex1)) {
                edgeVertexIndex1 = oldToNewEdgeIndexMap.at(edgeVertexIndex1);
            }
            uint32_t edgeVertexIndex2 = edge.vertexIndex2;
            while (oldToNewEdgeIndexMap.contains(edgeVertexIndex2)) {
                edgeVertexIndex2 = oldToNewEdgeIndexMap.at(edgeVertexIndex2);
            }

            Point3<float> edgePoint1 = newVertices[edgeVertexIndex1];
            Point3<float> edgePoint2 = newVertices[edgeVertexIndex2];
            float squareDistance = edgePoint1.squareDistance(edgePoint2);
            if (squareDistance > edgeSquareDistanceThreshold) {
                continue;
            }

            if (trianglesIndex.size() != 2) {
                continue; //the edge is not shared by 2 triangles (no-manifold mesh)
            }

            Point3<float> newPosition = (edgePoint1 + edgePoint2) / 2.0f;
            newVertices[edgeVertexIndex1] = newPosition;
            oldToNewEdgeIndexMap[edgeVertexIndex2] = edgeVertexIndex1;

            std::unordered_set<std::size_t> affectedTrianglesIndex = vertexToTriangles.at(edgeVertexIndex2);
            vertexToTriangles.erase(edgeVertexIndex2);
            for (std::size_t affectedTriangleIndex : affectedTrianglesIndex) {
                if (removedTriangles.contains(affectedTriangleIndex)) {
                    continue;
                }

                std::array<uint32_t, 3>& triangles = newTrianglesIndices[affectedTriangleIndex];
                for (std::size_t i = 0; i < 3; ++i) {
                    if (triangles[i] == edgeVertexIndex2) {
                        triangles[i] = edgeVertexIndex1; //TODO check triangle orientation !
                    }
                }

                if (triangles[0] == triangles[1] || triangles[1] == triangles[2] || triangles[2] == triangles[0]) {
                    removedTriangles.insert(affectedTriangleIndex); //remove degenerate triangle
                } else {
                    vertexToTriangles[edgeVertexIndex1].insert(affectedTriangleIndex);
                }
            }
        }

        std::vector<std::array<uint32_t, 3>> activeTriangles;
        activeTriangles.reserve(newTrianglesIndices.size() - removedTriangles.size());
        for (size_t i = 0; i < newTrianglesIndices.size(); ++i) {
            if (!removedTriangles.contains(i)) {
                activeTriangles.push_back(newTrianglesIndices[i]);
            }
        }
        newTrianglesIndices = std::move(activeTriangles);

        cleanUnusedVertices(newVertices, newTrianglesIndices);
        return MeshData(newVertices, newTrianglesIndices);
    }

    void MeshSimplificationService::cleanUnusedVertices(std::vector<Point3<float>>& vertices, std::vector<std::array<uint32_t, 3>>& trianglesIndices) const {
        std::unordered_set<uint32_t> usedIndices;
        for (const auto& triangleIndices : trianglesIndices) {
            usedIndices.insert(triangleIndices[0]);
            usedIndices.insert(triangleIndices[1]);
            usedIndices.insert(triangleIndices[2]);
        }

        std::unordered_map<uint32_t, uint32_t> oldToNew;
        std::vector<Point3<float>> newVertices;
        newVertices.reserve(usedIndices.size());

        for (uint32_t oldIndex = 0, newIndex = 0; oldIndex < vertices.size(); ++oldIndex) {
            if (usedIndices.contains(oldIndex)) {
                oldToNew[oldIndex] = newIndex++;
                newVertices.push_back(vertices[oldIndex]);
            }
        }

        for (std::array<uint32_t, 3>& triangleIndices : trianglesIndices) {
            triangleIndices[0] = oldToNew[triangleIndices[0]];
            triangleIndices[1] = oldToNew[triangleIndices[1]];
            triangleIndices[2] = oldToNew[triangleIndices[2]];
        }
        vertices = std::move(newVertices);
    }

}
