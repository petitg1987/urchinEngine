#include <utility>

#include <resources/geometry/frustum/FrustumModel.h>

namespace urchin {

    FrustumModel::FrustumModel(Frustum<float> frustum):
            frustum(std::move(frustum)) {

    }

    std::vector<Point3<float>> FrustumModel::retrieveVertexArray(std::vector<uint32_t>& indices) const {
        std::array<Point3<float>, 8> frustumPoints = frustum.getFrustumPoints();
        std::vector vertexArray(frustumPoints.begin(), frustumPoints.end());

        indices.reserve(6ul * 6ul);
        //back
        indices.push_back(6);
        indices.push_back(7);
        indices.push_back(4);

        indices.push_back(4);
        indices.push_back(7);
        indices.push_back(5);

        //front
        indices.push_back(3);
        indices.push_back(2);
        indices.push_back(0);

        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(3);

        //left
        indices.push_back(0);
        indices.push_back(2);
        indices.push_back(6);

        indices.push_back(6);
        indices.push_back(4);
        indices.push_back(0);

        //right
        indices.push_back(3);
        indices.push_back(1);
        indices.push_back(7);

        indices.push_back(7);
        indices.push_back(1);
        indices.push_back(5);

        //bottom
        indices.push_back(6);
        indices.push_back(2);
        indices.push_back(3);

        indices.push_back(3);
        indices.push_back(7);
        indices.push_back(6);

        //top
        indices.push_back(1);
        indices.push_back(0);
        indices.push_back(4);

        indices.push_back(4);
        indices.push_back(5);
        indices.push_back(1);

        return vertexArray;
    }

    std::vector<LineSegment3D<float>> FrustumModel::retrieveWireframeLines() const {
        const std::array<Point3<float>, 8>& points = frustum.getFrustumPoints();

        std::vector<LineSegment3D<float>> lines;
        lines.reserve(12ul);

        //front
        lines.emplace_back(points[1], points[3]);
        lines.emplace_back(points[3], points[2]);
        lines.emplace_back(points[2], points[0]);
        lines.emplace_back(points[0], points[1]);

        //back
        lines.emplace_back(points[5], points[7]);
        lines.emplace_back(points[7], points[6]);
        lines.emplace_back(points[6], points[4]);
        lines.emplace_back(points[4], points[5]);

        //sides
        lines.emplace_back(points[1], points[5]);
        lines.emplace_back(points[3], points[7]);
        lines.emplace_back(points[2], points[6]);
        lines.emplace_back(points[0], points[4]);

        return lines;
    }

    ShapeType FrustumModel::getShapeType() const {
        return ShapeType::TRIANGLE;
    }

}
