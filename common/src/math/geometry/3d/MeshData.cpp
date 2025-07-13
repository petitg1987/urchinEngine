#include <math/geometry/3d/MeshData.h>

namespace urchin {

    MeshData::MeshData(std::vector<Point3<float>> vertices, std::vector<std::array<uint32_t, 3>> trianglesIndices) :
            vertices(std::move(vertices)),
            trianglesIndices(std::move(trianglesIndices))
    {

    }

    void MeshData::addNewMesh(const std::vector<Point3<float>>& newVertices, const std::vector<std::array<uint32_t, 3>>& newTrianglesIndices) {
        uint32_t indexIncrementValue = (uint32_t)vertices.size();
        vertices.insert(vertices.end(), newVertices.begin(), newVertices.end());
        for (const std::array<uint32_t, 3> newTriangleIndices : newTrianglesIndices) {
            trianglesIndices.push_back({newTriangleIndices[0] + indexIncrementValue, newTriangleIndices[1] + indexIncrementValue, newTriangleIndices[2] + indexIncrementValue});
        }
    }

    const std::vector<Point3<float>>& MeshData::getVertices() const {
        return vertices;
    }

    const std::vector<std::array<uint32_t, 3>>& MeshData::getTrianglesIndices() const {
        return trianglesIndices;
    }

}