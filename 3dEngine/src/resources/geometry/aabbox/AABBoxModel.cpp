#include <utility>

#include <resources/geometry/aabbox/AABBoxModel.h>

namespace urchin {

    AABBoxModel::AABBoxModel(const AABBox<float>& aabbox):
            aabboxes({aabbox}) {

    }

    AABBoxModel::AABBoxModel(std::vector<AABBox<float>> aabboxes):
            aabboxes(std::move(aabboxes)) {

    }

    std::vector<Point3<float>> AABBoxModel::retrieveVertexArray(std::vector<uint32_t>& indices) const {
        indices.reserve(aabboxes.size() * 6 * 6);
        std::vector<Point3<float>> vertexArray;
        vertexArray.reserve(aabboxes.size() * 8);

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

    ShapeType AABBoxModel::getShapeType() const {
        return ShapeType::TRIANGLE;
    }

}
