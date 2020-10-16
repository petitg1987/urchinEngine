#include <GL/glew.h>

#include <utility>

#include "TrianglesModel.h"

namespace urchin {

    TrianglesModel::TrianglesModel(std::vector<Point3<float>> trianglesPoints) :
        trianglesPoints(std::move(trianglesPoints)) {
        initialize();
    }

    Matrix4<float> TrianglesModel::retrieveModelMatrix() const {
        return {};
    }

    std::vector<Point3<float>> TrianglesModel::retrieveVertexArray() const {
        return trianglesPoints;
    }

    void TrianglesModel::drawGeometry() const {
        glDrawArrays(GL_TRIANGLES, 0, trianglesPoints.size());
    }

}
