#include <utility>

#include <resources/geometry/line/LineModel.h>

namespace urchin {

    LineModel::LineModel(const LineSegment3D<float>& line, float lineWidth):
            lines({line}) {
        setWireframeLineWidth(lineWidth);
    }

    LineModel::LineModel(std::vector<LineSegment3D<float>> lines, float lineWidth):
            lines(std::move(lines)) {
        setWireframeLineWidth(lineWidth);
    }

    std::vector<Point3<float>> LineModel::retrieveVertexArray(std::vector<uint32_t>& indices) const {
        if (getPolygonMode() == PolygonMode::WIREFRAME) {
            throw std::runtime_error("Wireframe polygon mode is not allowed on line model");
        }

        std::vector<Point4<float>> vertexData;
        return linesToVertexArray(lines, indices, vertexData); //TODO review
    }

    ShapeType LineModel::getShapeType() const {
        return ShapeType::TRIANGLE;
    }

}
