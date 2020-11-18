#include <utility>

#include "QuadsModel.h"

namespace urchin {

    QuadsModel::QuadsModel(std::vector<Point3<float>> quadsPoints) :
            quadsPoints(std::move(quadsPoints)) {
        if(this->quadsPoints.size() % 4 != 0) {
            throw std::invalid_argument("Quads points size must a multiple of four. Size: " + std::to_string(this->quadsPoints.size()));
        }
        initialize();
    }

    Matrix4<float> QuadsModel::retrieveModelMatrix() const {
        return {};
    }

    std::vector<Point3<float>> QuadsModel::retrieveVertexArray() const {
        return quadsPoints;
    }

    ShapeType QuadsModel::getShapeType() const {
        return ShapeType::RECTANGLE;
    }

}
