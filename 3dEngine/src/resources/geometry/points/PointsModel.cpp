#include <utility>

#include "resources/geometry/points/PointsModel.h"

namespace urchin {

    PointsModel::PointsModel(std::vector<Point3<float>> points):
            points(std::move(points)) {
        initialize();
    }

    PointsModel::PointsModel(const Point3<float>& point) {
        points.push_back(point);

        initialize();
    }

    Matrix4<float> PointsModel::retrieveModelMatrix() const {
        return {};
    }

    std::vector<Point3<float>> PointsModel::retrieveVertexArray() const {
        return points;
    }

    ShapeType PointsModel::getShapeType() const {
        return ShapeType::POINT;
    }

}
