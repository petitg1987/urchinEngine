#include <utility>

#include <resources/geometry/obbox/OBBoxModel.h>

namespace urchin {

    OBBoxModel::OBBoxModel(OBBox<float> obbox):
            obbox(std::move(obbox)) {

    }

    std::vector<Point3<float>> OBBoxModel::retrieveVertexArray(std::vector<uint32_t>& indices) const {
        indices.reserve(6 * 6);
        std::vector<Point3<float>> vertexArray;
        vertexArray.reserve(8);

        for(unsigned int i = 0; i < 8; ++i) {
            vertexArray.push_back(obbox.getPoint(i));
        }
        
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

    ShapeType OBBoxModel::getShapeType() const {
        return ShapeType::TRIANGLE;
    }

}
