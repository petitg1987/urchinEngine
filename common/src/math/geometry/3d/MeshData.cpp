#include <math/geometry/3d/MeshData.h>

namespace urchin {

    MeshData::MeshData(std::vector<Point3<float>> vertices, std::vector<std::array<uint32_t, 3>> trianglesIndices) :
            vertices(std::move(vertices)),
            trianglesIndices(std::move(trianglesIndices))
    {

    }

    const std::vector<Point3<float>>& MeshData::getVertices() const {
        return vertices;
    }

    const std::vector<std::array<uint32_t, 3>>& MeshData::getTrianglesIndices() const {
        return trianglesIndices;
    }

}