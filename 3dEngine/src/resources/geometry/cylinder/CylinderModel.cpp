#include <GL/glew.h>
#include <cmath>
#include <utility>

#include "resources/geometry/cylinder/CylinderModel.h"

namespace urchin {

    CylinderModel::CylinderModel(Cylinder<float> cylinder, int sides):
            cylinder(std::move(cylinder)),
            sides(sides) {
        initialize();
    }

    Matrix4<float> CylinderModel::retrieveModelMatrix() const {
        Matrix4<float> modelMatrix;
        modelMatrix.buildTranslation(cylinder.getCenterOfMass().X, cylinder.getCenterOfMass().Y, cylinder.getCenterOfMass().Z);

        return modelMatrix;
    }

    std::vector<Point3<float>> CylinderModel::retrieveVertexArray() const {
        std::vector<Point3<float>> vertexArray;
        vertexArray.reserve(4*sides);

        float radius = cylinder.getRadius();
        float halfHeight = cylinder.getHeight() / 2.0f;
        float angle = (2.0f * (float)PI_VALUE) / (float)sides;

        Quaternion<float> qCylinderOrientation;
        CylinderShape<float>::CylinderOrientation cylinderOrientation = cylinder.getCylinderOrientation();
        if (cylinderOrientation == CylinderShape<float>::CYLINDER_X) {
            qCylinderOrientation = Quaternion<float>(Vector3<float>(0.0, 1.0, 0.0), PI_VALUE/2.0);
        } else if (cylinderOrientation == CylinderShape<float>::CYLINDER_Y) {
            qCylinderOrientation = Quaternion<float>(Vector3<float>(1.0, 0.0, 0.0), PI_VALUE/2.0);
        } else if (cylinderOrientation == CylinderShape<float>::CYLINDER_Z) {
            qCylinderOrientation = Quaternion<float>(0.0, 0.0, 0.0, 1.0);
        }

        Quaternion<float> localOrientation = cylinder.getOrientation() * qCylinderOrientation;
        for (int i = 0; i < sides; i++) {
            float x1 = std::cos((float)i * angle) * radius;
            float y1 = std::sin((float)i * angle) * radius;
            float x2 = std::cos((float)(i+1) * angle) * radius;
            float y2 = std::sin((float)(i+1) * angle) * radius;

            vertexArray.push_back(localOrientation.rotatePoint(Point3<float>(x1, y1, halfHeight)));
            vertexArray.push_back(localOrientation.rotatePoint(Point3<float>(x1, y1, -halfHeight)));
            vertexArray.push_back(localOrientation.rotatePoint(Point3<float>(x2, y2, -halfHeight)));
            vertexArray.push_back(localOrientation.rotatePoint(Point3<float>(x2, y2, halfHeight)));
        }

        return vertexArray;
    }

    void CylinderModel::drawGeometry() const {
        glDrawArrays(GL_QUADS, 0, 4*sides);
    }

}
