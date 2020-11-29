#include <utility>

#include "LinesModel.h"

namespace urchin {

    LinesModel::LinesModel(std::vector<Point3<float>> stripLinesPoints) :
            linesPoints(std::move(stripLinesPoints)),
            stripLines(true) {
        if(this->linesPoints.size() != 1) {
            throw std::invalid_argument("Lines points size must different from one.");
        }
        initialize();
    }

    LinesModel::LinesModel(const std::vector<LineSegment3D<float>> &lineSegments) :
            stripLines(false) {
        linesPoints.reserve(lineSegments.size() * 2);
        for (const auto &lineSegment : lineSegments) {
            linesPoints.emplace_back(lineSegment.getA());
            linesPoints.emplace_back(lineSegment.getB());
        }

        initialize();
    }

    LinesModel::LinesModel(const LineSegment3D<float>& lineSegment) :
        stripLines(true){
        linesPoints.push_back(lineSegment.getA());
        linesPoints.push_back(lineSegment.getB());

        initialize();
    }

    Matrix4<float> LinesModel::retrieveModelMatrix() const {
        return {};
    }

    std::vector<Point3<float>> LinesModel::retrieveVertexArray() const {
        return linesPoints;
    }

    ShapeType LinesModel::getShapeType() const {
        return stripLines ? ShapeType::LINE_STRIP : ShapeType::LINE;
    }
}
