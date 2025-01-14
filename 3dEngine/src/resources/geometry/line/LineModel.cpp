#include <utility>

#include <resources/geometry/line/LineModel.h>

namespace urchin {

    LineModel::LineModel(const LineSegment3D<float>& line, float lineWidth):
            lines({line}) {
        setPolygonMode(PolygonMode::WIREFRAME);
        setWireframeLineWidth(lineWidth);
    }

    LineModel::LineModel(std::vector<LineSegment3D<float>> lines, float lineWidth):
            lines(std::move(lines)) {
        setPolygonMode(PolygonMode::WIREFRAME);
        setWireframeLineWidth(lineWidth);
    }

    std::vector<Point3<float>> LineModel::retrieveVertexArray(std::vector<uint32_t>&) const {
        throw std::runtime_error("Polygon mode must be defined to wireframe");
    }

    std::vector<LineSegment3D<float>> LineModel::retrieveWireframeLines() const {
        return lines;
    }

    ShapeType LineModel::getShapeType() const {
        throw std::runtime_error("Polygon mode must be defined to wireframe");
    }

}
