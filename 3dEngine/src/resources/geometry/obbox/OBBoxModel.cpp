#include <utility>

#include <resources/geometry/obbox/OBBoxModel.h>

namespace urchin {

    OBBoxModel::OBBoxModel(OBBox<float> obbox):
            obbox(std::move(obbox)) {

    }

    std::vector<Point3<float>> OBBoxModel::retrieveVertexArray() const {
        std::vector<Point3<float>> vertexArray;
        vertexArray.reserve(6 * 6);

        //back
        vertexArray.push_back(obbox.getPoint(7));
        vertexArray.push_back(obbox.getPoint(3));
        vertexArray.push_back(obbox.getPoint(5));

        vertexArray.push_back(obbox.getPoint(5));
        vertexArray.push_back(obbox.getPoint(3));
        vertexArray.push_back(obbox.getPoint(1));

        //front
        vertexArray.push_back(obbox.getPoint(2));
        vertexArray.push_back(obbox.getPoint(6));
        vertexArray.push_back(obbox.getPoint(4));

        vertexArray.push_back(obbox.getPoint(4));
        vertexArray.push_back(obbox.getPoint(0));
        vertexArray.push_back(obbox.getPoint(2));

        //left
        vertexArray.push_back(obbox.getPoint(4));
        vertexArray.push_back(obbox.getPoint(6));
        vertexArray.push_back(obbox.getPoint(7));

        vertexArray.push_back(obbox.getPoint(7));
        vertexArray.push_back(obbox.getPoint(5));
        vertexArray.push_back(obbox.getPoint(4));

        //right
        vertexArray.push_back(obbox.getPoint(2));
        vertexArray.push_back(obbox.getPoint(0));
        vertexArray.push_back(obbox.getPoint(3));

        vertexArray.push_back(obbox.getPoint(3));
        vertexArray.push_back(obbox.getPoint(0));
        vertexArray.push_back(obbox.getPoint(1));

        //bottom
        vertexArray.push_back(obbox.getPoint(7));
        vertexArray.push_back(obbox.getPoint(6));
        vertexArray.push_back(obbox.getPoint(2));

        vertexArray.push_back(obbox.getPoint(2));
        vertexArray.push_back(obbox.getPoint(3));
        vertexArray.push_back(obbox.getPoint(7));

        //top
        vertexArray.push_back(obbox.getPoint(0));
        vertexArray.push_back(obbox.getPoint(4));
        vertexArray.push_back(obbox.getPoint(5));

        vertexArray.push_back(obbox.getPoint(5));
        vertexArray.push_back(obbox.getPoint(1));
        vertexArray.push_back(obbox.getPoint(0));

        return vertexArray;
    }

    ShapeType OBBoxModel::getShapeType() const {
        return ShapeType::TRIANGLE;
    }

}
