#include <utility>

#include <resources/geometry/aabbox/AABBoxModel.h>

namespace urchin {

    AABBoxModel::AABBoxModel(const AABBox<float>& aabbox):
            aabboxes({aabbox}) {

    }

    AABBoxModel::AABBoxModel(std::vector<AABBox<float>> aabboxes):
            aabboxes(std::move(aabboxes)) {

    }

    std::vector<Point3<float>> AABBoxModel::retrieveVertexArray() const {
        std::vector<Point3<float>> vertexArray;
        vertexArray.reserve(6 * 6 * aabboxes.size());

        for (const auto& aabbox : aabboxes) {
            //Points of AABBox sorted first on positive X axis, then on positive Y axis and then on positive Z axis
            std::array<Point3<float>, 8> aabboxPoints = {
                    Point3<float>(aabbox.getMax().X, aabbox.getMax().Y, aabbox.getMax().Z),
                    Point3<float>(aabbox.getMax().X, aabbox.getMax().Y, aabbox.getMin().Z),
                    Point3<float>(aabbox.getMax().X, aabbox.getMin().Y, aabbox.getMax().Z),
                    Point3<float>(aabbox.getMax().X, aabbox.getMin().Y, aabbox.getMin().Z),
                    Point3<float>(aabbox.getMin().X, aabbox.getMax().Y, aabbox.getMax().Z),
                    Point3<float>(aabbox.getMin().X, aabbox.getMax().Y, aabbox.getMin().Z),
                    Point3<float>(aabbox.getMin().X, aabbox.getMin().Y, aabbox.getMax().Z),
                    Point3<float>(aabbox.getMin().X, aabbox.getMin().Y, aabbox.getMin().Z),
            };

            //back
            vertexArray.push_back(aabboxPoints[7]);
            vertexArray.push_back(aabboxPoints[3]);
            vertexArray.push_back(aabboxPoints[5]);

            vertexArray.push_back(aabboxPoints[5]);
            vertexArray.push_back(aabboxPoints[3]);
            vertexArray.push_back(aabboxPoints[1]);

            //front
            vertexArray.push_back(aabboxPoints[2]);
            vertexArray.push_back(aabboxPoints[6]);
            vertexArray.push_back(aabboxPoints[4]);

            vertexArray.push_back(aabboxPoints[4]);
            vertexArray.push_back(aabboxPoints[0]);
            vertexArray.push_back(aabboxPoints[2]);

            //left
            vertexArray.push_back(aabboxPoints[4]);
            vertexArray.push_back(aabboxPoints[6]);
            vertexArray.push_back(aabboxPoints[7]);

            vertexArray.push_back(aabboxPoints[7]);
            vertexArray.push_back(aabboxPoints[5]);
            vertexArray.push_back(aabboxPoints[4]);

            //right
            vertexArray.push_back(aabboxPoints[2]);
            vertexArray.push_back(aabboxPoints[0]);
            vertexArray.push_back(aabboxPoints[3]);

            vertexArray.push_back(aabboxPoints[3]);
            vertexArray.push_back(aabboxPoints[0]);
            vertexArray.push_back(aabboxPoints[1]);

            //bottom
            vertexArray.push_back(aabboxPoints[7]);
            vertexArray.push_back(aabboxPoints[6]);
            vertexArray.push_back(aabboxPoints[2]);

            vertexArray.push_back(aabboxPoints[2]);
            vertexArray.push_back(aabboxPoints[3]);
            vertexArray.push_back(aabboxPoints[7]);

            //top
            vertexArray.push_back(aabboxPoints[0]);
            vertexArray.push_back(aabboxPoints[4]);
            vertexArray.push_back(aabboxPoints[5]);

            vertexArray.push_back(aabboxPoints[5]);
            vertexArray.push_back(aabboxPoints[1]);
            vertexArray.push_back(aabboxPoints[0]);
        }

        return vertexArray;
    }

    ShapeType AABBoxModel::getShapeType() const {
        return ShapeType::TRIANGLE;
    }

}
