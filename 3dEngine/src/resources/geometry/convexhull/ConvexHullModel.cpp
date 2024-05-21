#include <utility>

#include <resources/geometry/convexhull/ConvexHullModel.h>

namespace urchin {

    ConvexHullModel::ConvexHullModel(ConvexHull3D<float> convexHull):
            convexHull(std::move(convexHull)) {

    }

    std::vector<Point3<float>> ConvexHullModel::retrieveVertexArray(std::vector<uint32_t>&) const {
        std::vector<Point3<float>> vertexArray;
        vertexArray.reserve(convexHull.getIndexedTriangles().size() * 3ul);

        auto& convexHullPoints = convexHull.getConvexHullPoints();
        for (const auto& [triangleId, triangle] : convexHull.getIndexedTriangles()) {
            vertexArray.push_back(convexHullPoints.at(triangle.getIndex(0)).point);
            vertexArray.push_back(convexHullPoints.at(triangle.getIndex(2)).point);
            vertexArray.push_back(convexHullPoints.at(triangle.getIndex(1)).point);
        }

        return vertexArray;
    }

    ShapeType ConvexHullModel::getShapeType() const {
        return ShapeType::TRIANGLE;
    }

}
