#include <GL/glew.h>
#include <utility>

#include "QuadsModel.h"

namespace urchin {

    QuadsModel::QuadsModel(std::vector<Point3<float>> quadsPoints) :
            quadsPoints(std::move(quadsPoints)) {
        initialize();
    }

    Matrix4<float> QuadsModel::retrieveModelMatrix() const {
        return {};
    }

    std::vector<Point3<float>> QuadsModel::retrieveVertexArray() const {
        return quadsPoints;
    }

    void QuadsModel::drawGeometry() const {
        glDrawArrays(GL_QUADS, 0, quadsPoints.size());
    }

}
