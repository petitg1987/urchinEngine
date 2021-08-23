#include <utility>

#include <resources/geometry/triangles/TrianglesModel.h>

namespace urchin {

    TrianglesModel::TrianglesModel(std::vector<Point3<float>> trianglesPoints) :
            trianglesPoints(std::move(trianglesPoints)) {
        if (this->trianglesPoints.size() % 3 != 0) {
            throw std::invalid_argument("Triangles points size must a multiple of three. Size: " + std::to_string(this->trianglesPoints.size()));
        }
    }

    std::vector<Point3<float>> TrianglesModel::retrieveVertexArray() const {
        return trianglesPoints;
    }

    ShapeType TrianglesModel::getShapeType() const {
        return ShapeType::TRIANGLE;
    }

}
