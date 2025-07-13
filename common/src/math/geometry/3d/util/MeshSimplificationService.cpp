#include <unordered_map>

#include <math/geometry/3d/util/MeshSimplificationService.h>

namespace urchin {

    MeshSimplificationService::MeshSimplificationService(Config config) :
            config(std::move(config)) {

    }

    MeshData MeshSimplificationService::simplify(const MeshData& meshData) const {
        MeshData meshUniqueVertices = mergeDuplicateVertices(meshData);

        //TODO Simple Edge Collapse Algorithm

        return meshUniqueVertices;
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

}
