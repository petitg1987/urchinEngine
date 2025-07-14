#include <unordered_map>
#include <unordered_set>

#include <math/geometry/3d/util/MeshSimplificationService.h>

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
        std::unordered_map<uint32_t, std::set<std::size_t>> vertexToTriangles;
        for (std::size_t triangleIndex = 0; triangleIndex < mesh.getTrianglesIndices().size(); ++triangleIndex) {
            const std::array<uint32_t, 3>& triangleIndices = mesh.getTrianglesIndices()[triangleIndex];

            edgeToTriangles[buildEdge(triangleIndices[0], triangleIndices[1])].push_back(triangleIndex);
            edgeToTriangles[buildEdge(triangleIndices[1], triangleIndices[2])].push_back(triangleIndex);
            edgeToTriangles[buildEdge(triangleIndices[2], triangleIndices[0])].push_back(triangleIndex);

            vertexToTriangles[triangleIndices[0]].insert(triangleIndex);
            vertexToTriangles[triangleIndices[1]].insert(triangleIndex);
            vertexToTriangles[triangleIndices[2]].insert(triangleIndex);
        }

        std::unordered_set<std::size_t> removedTriangles;
        float edgeSquareDistanceThreshold = config.edgeDistanceThreshold * config.edgeDistanceThreshold;
        for (const auto& [edge, trianglesIndex] : edgeToTriangles) {
            if (edge.vertexIndex1 == edge.vertexIndex2) { //v1 & v2
                continue; //TODO possible ?
            }

            Point3<float> edgePoint1 = mesh.getVertices()[edge.vertexIndex1];
            Point3<float> edgePoint2 = mesh.getVertices()[edge.vertexIndex1];
            float squareDistance = edgePoint1.squareDistance(edgePoint2);
            if (squareDistance > edgeSquareDistanceThreshold) {
                continue;
            }

            if (trianglesIndex.size() != 2) {
                continue; //the edge is not shared by 2 triangles (no-manifold mesh)
            }

            Point3<float> newPosition = (edgePoint1 + edgePoint2) / 2.0f;
            newVertices[edge.vertexIndex1] = newPosition;

            std::set<std::size_t> affectedTrianglesIndex = vertexToTriangles.at(edge.vertexIndex2);
            for (std::size_t affectedTriangleIndex : affectedTrianglesIndex) {
                std::array<uint32_t, 3>& triangles = newTrianglesIndices[affectedTriangleIndex];
                for (std::size_t i = 0; i < 3; ++i) {
                    if (triangles[i] == edge.vertexIndex2) {
                        triangles[i] = edge.vertexIndex1; //TODO check triangle orientation !
                    }
                }

                if (triangles[0] == triangles[1] || triangles[1] == triangles[2] || triangles[0] == triangles[2]) { //TODO possible ?
                    removedTriangles.insert(affectedTriangleIndex); //remove degenerate triangle
                } else {
                    vertexToTriangles[edge.vertexIndex1].insert(affectedTriangleIndex);
                }
            }

            for (std::size_t triangleIndex : trianglesIndex) {
                removedTriangles.insert(triangleIndex); //remove the two triangles which used the edge collapsed
            }
        }

        std::erase_if(newTrianglesIndices,[&removedTriangles](std::size_t t) { return removedTriangles.contains(t); });

        //TODO remove vertex unused !

        return MeshData(newVertices, newTrianglesIndices);
    }

}
