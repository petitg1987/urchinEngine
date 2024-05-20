#include <utility>

#include <resources/geometry/line/LineModel.h>

namespace urchin {

    LineModel::LineModel(const LineSegment3D<float>& line, float lineWidth):
            lines({line}),
            lineWidth(lineWidth) {

    }

    LineModel::LineModel(std::vector<LineSegment3D<float>> lines, float lineWidth):
            lines(std::move(lines)),
            lineWidth(lineWidth) {

    }

    std::vector<Point3<float>> LineModel::retrieveVertexArray(std::vector<uint32_t>& indices) const {
        indices.reserve(lines.size() * 12);
        std::vector<Point3<float>> vertexArray;
        vertexArray.reserve(lines.size() * 6);

        float degree120 = AngleConverter<float>::toRadian(120);
        for (const auto& line : lines) {
            Vector3<float> lineVector = line.toVector().normalize();
            if (lineVector.squareLength() < 0.001f) {
                continue;
            }
            Quaternion<float> qRotation = Quaternion<float>::fromAxisAngle(lineVector, degree120);

            Vector3<float> perpendicularVector1 = lineVector.perpendicularVector().normalize() * lineWidth;
            Vector3<float> perpendicularVector2 = qRotation.rotateVector(perpendicularVector1);
            Vector3<float> perpendicularVector3 = qRotation.rotateVector(perpendicularVector2);

            vertexArray.emplace_back(line.getA().translate(perpendicularVector1));
            vertexArray.emplace_back(line.getA().translate(perpendicularVector2));
            vertexArray.emplace_back(line.getA().translate(perpendicularVector3));

            vertexArray.emplace_back(line.getB().translate(perpendicularVector1));
            vertexArray.emplace_back(line.getB().translate(perpendicularVector2));
            vertexArray.emplace_back(line.getB().translate(perpendicularVector3));
        }

        for (uint32_t i = 0; i < lines.size(); ++i) {
            uint32_t startIndex = i * 6;
            
            //face 1
            indices.push_back(startIndex + 3);
            indices.push_back(startIndex + 1);
            indices.push_back(startIndex + 0);

            indices.push_back(startIndex + 1);
            indices.push_back(startIndex + 3);
            indices.push_back(startIndex + 4);

            //face 2
            indices.push_back(startIndex + 4);
            indices.push_back(startIndex + 2);
            indices.push_back(startIndex + 1);

            indices.push_back(startIndex + 2);
            indices.push_back(startIndex + 4);
            indices.push_back(startIndex + 5);

            //face 3
            indices.push_back(startIndex + 5);
            indices.push_back(startIndex + 0);
            indices.push_back(startIndex + 2);

            indices.push_back(startIndex + 0);
            indices.push_back(startIndex + 5);
            indices.push_back(startIndex + 3);
        }

        return vertexArray;
    }

    ShapeType LineModel::getShapeType() const {
        return ShapeType::TRIANGLE;
    }

}
