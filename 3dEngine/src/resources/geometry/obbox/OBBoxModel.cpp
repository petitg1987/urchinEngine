#include <utility>

#include <resources/geometry/obbox/OBBoxModel.h>

namespace urchin {

    OBBoxModel::OBBoxModel(OBBox<float> obbox):
            obbox(std::move(obbox)) {

    }

    std::vector<Point3<float>> OBBoxModel::retrieveVertexArray(std::vector<uint32_t>& indices) const {
        std::vector<Point3<float>> vertexArray = obbox.getPoints();

        indices.reserve(6ul * 6ul);
        //back
        indices.push_back(7);
        indices.push_back(3);
        indices.push_back(5);

        indices.push_back(5);
        indices.push_back(3);
        indices.push_back(1);

        //front
        indices.push_back(2);
        indices.push_back(6);
        indices.push_back(4);

        indices.push_back(4);
        indices.push_back(0);
        indices.push_back(2);

        //left
        indices.push_back(4);
        indices.push_back(6);
        indices.push_back(7);

        indices.push_back(7);
        indices.push_back(5);
        indices.push_back(4);

        //right
        indices.push_back(2);
        indices.push_back(0);
        indices.push_back(3);

        indices.push_back(3);
        indices.push_back(0);
        indices.push_back(1);

        //bottom
        indices.push_back(7);
        indices.push_back(6);
        indices.push_back(2);

        indices.push_back(2);
        indices.push_back(3);
        indices.push_back(7);

        //top
        indices.push_back(0);
        indices.push_back(4);
        indices.push_back(5);

        indices.push_back(5);
        indices.push_back(1);
        indices.push_back(0);

        return vertexArray;
    }

    std::vector<LineSegment3D<float>> OBBoxModel::retrieveWireframeLines() const {
        std::vector<Point3<float>> points = obbox.getPoints();

        std::vector<LineSegment3D<float>> lines;
        lines.reserve(12ul);

        //front
        lines.emplace_back(points[0], points[2]);
        lines.emplace_back(points[2], points[6]);
        lines.emplace_back(points[6], points[4]);
        lines.emplace_back(points[4], points[0]);

        //back
        lines.emplace_back(points[1], points[3]);
        lines.emplace_back(points[3], points[7]);
        lines.emplace_back(points[7], points[5]);
        lines.emplace_back(points[5], points[1]);

        //sides
        lines.emplace_back(points[0], points[1]);
        lines.emplace_back(points[2], points[3]);
        lines.emplace_back(points[6], points[7]);
        lines.emplace_back(points[4], points[5]);

        return lines;
    }

    ShapeType OBBoxModel::getShapeType() const {
        return ShapeType::TRIANGLE;
    }

}
