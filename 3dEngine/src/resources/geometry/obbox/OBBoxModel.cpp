#include <GL/glew.h>

#include <utility>

#include "resources/geometry/obbox/OBBoxModel.h"

namespace urchin {

    OBBoxModel::OBBoxModel(OBBox<float> obbox):
            obbox(std::move(obbox)) {
        initialize();
    }

    Matrix4<float> OBBoxModel::retrieveModelMatrix() const {
        return {};
    }

    std::vector<Point3<float>> OBBoxModel::retrieveVertexArray() const {
        std::vector<Point3<float>> vertexArray;
        vertexArray.reserve(24);

        vertexArray.push_back(obbox.getPoint(0));
        vertexArray.push_back(obbox.getPoint(1));

        vertexArray.push_back(obbox.getPoint(1));
        vertexArray.push_back(obbox.getPoint(3));

        vertexArray.push_back(obbox.getPoint(3));
        vertexArray.push_back(obbox.getPoint(2));

        vertexArray.push_back(obbox.getPoint(2));
        vertexArray.push_back(obbox.getPoint(0));


        vertexArray.push_back(obbox.getPoint(1));
        vertexArray.push_back(obbox.getPoint(5));

        vertexArray.push_back(obbox.getPoint(3));
        vertexArray.push_back(obbox.getPoint(7));

        vertexArray.push_back(obbox.getPoint(2));
        vertexArray.push_back(obbox.getPoint(6));

        vertexArray.push_back(obbox.getPoint(0));
        vertexArray.push_back(obbox.getPoint(4));


        vertexArray.push_back(obbox.getPoint(4));
        vertexArray.push_back(obbox.getPoint(5));

        vertexArray.push_back(obbox.getPoint(5));
        vertexArray.push_back(obbox.getPoint(7));

        vertexArray.push_back(obbox.getPoint(7));
        vertexArray.push_back(obbox.getPoint(6));

        vertexArray.push_back(obbox.getPoint(6));
        vertexArray.push_back(obbox.getPoint(4));

        return vertexArray;
    }

    void OBBoxModel::drawGeometry() const {
        glDrawArrays(GL_LINES, 0, 24);
    }

}
