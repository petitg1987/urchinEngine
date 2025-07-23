#include <utility>

#include "resources/geometry/aabbox/AABBoxModel.h"

namespace urchin {

    AABBoxModel::AABBoxModel(const AABBox<float>& aabbox):
            aabboxes({aabbox}) {

    }

    AABBoxModel::AABBoxModel(std::vector<AABBox<float>> aabboxes):
            aabboxes(std::move(aabboxes)) {

    }

    std::vector<Point3<float>> AABBoxModel::retrieveVertexArray(std::vector<uint32_t>& indices) const {
        std::vector<Point3<float>> vertexArray;
        vertexArray.reserve(8ul * aabboxes.size());
        for (const auto& aabbox : aabboxes) {
            vertexArray.emplace_back(aabbox.getMax().X, aabbox.getMax().Y, aabbox.getMax().Z);
            vertexArray.emplace_back(aabbox.getMax().X, aabbox.getMax().Y, aabbox.getMin().Z);
            vertexArray.emplace_back(aabbox.getMax().X, aabbox.getMin().Y, aabbox.getMax().Z);
            vertexArray.emplace_back(aabbox.getMax().X, aabbox.getMin().Y, aabbox.getMin().Z);
            vertexArray.emplace_back(aabbox.getMin().X, aabbox.getMax().Y, aabbox.getMax().Z);
            vertexArray.emplace_back(aabbox.getMin().X, aabbox.getMax().Y, aabbox.getMin().Z);
            vertexArray.emplace_back(aabbox.getMin().X, aabbox.getMin().Y, aabbox.getMax().Z);
            vertexArray.emplace_back(aabbox.getMin().X, aabbox.getMin().Y, aabbox.getMin().Z);
        }

        indices.reserve(6ul * 6ul * aabboxes.size());
        for (uint32_t i = 0; i < aabboxes.size(); ++i) {
            uint32_t startIndex = i * 8;
            
            //back
            indices.push_back(startIndex + 7);
            indices.push_back(startIndex + 3);
            indices.push_back(startIndex + 5);

            indices.push_back(startIndex + 5);
            indices.push_back(startIndex + 3);
            indices.push_back(startIndex + 1);

            //front
            indices.push_back(startIndex + 2);
            indices.push_back(startIndex + 6);
            indices.push_back(startIndex + 4);

            indices.push_back(startIndex + 4);
            indices.push_back(startIndex + 0);
            indices.push_back(startIndex + 2);

            //left
            indices.push_back(startIndex + 4);
            indices.push_back(startIndex + 6);
            indices.push_back(startIndex + 7);

            indices.push_back(startIndex + 7);
            indices.push_back(startIndex + 5);
            indices.push_back(startIndex + 4);

            //right
            indices.push_back(startIndex + 2);
            indices.push_back(startIndex + 0);
            indices.push_back(startIndex + 3);

            indices.push_back(startIndex + 3);
            indices.push_back(startIndex + 0);
            indices.push_back(startIndex + 1);

            //bottom
            indices.push_back(startIndex + 7);
            indices.push_back(startIndex + 6);
            indices.push_back(startIndex + 2);

            indices.push_back(startIndex + 2);
            indices.push_back(startIndex + 3);
            indices.push_back(startIndex + 7);

            //top
            indices.push_back(startIndex + 0);
            indices.push_back(startIndex + 4);
            indices.push_back(startIndex + 5);

            indices.push_back(startIndex + 5);
            indices.push_back(startIndex + 1);
            indices.push_back(startIndex + 0);
        }

        return vertexArray;
    }

    std::vector<LineSegment3D<float>> AABBoxModel::retrieveWireframeLines() const {
        std::vector<LineSegment3D<float>> lines;
        lines.reserve(12ul * aabboxes.size());

        for (const auto& aabbox : aabboxes) {
            std::array<Point3<float>, 8> points = aabbox.getPoints();

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
        }

        return lines;
    }

    ShapeType AABBoxModel::getShapeType() const {
        return ShapeType::TRIANGLE;
    }

}
