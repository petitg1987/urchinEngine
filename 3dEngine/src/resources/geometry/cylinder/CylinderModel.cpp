#include <cmath>
#include <utility>

#include <resources/geometry/cylinder/CylinderModel.h>

namespace urchin {

    CylinderModel::CylinderModel(Cylinder<float> cylinder, unsigned int sides):
            cylinder(std::move(cylinder)),
            sides(sides) {

    }

    std::vector<Point3<float>> CylinderModel::retrieveVertexArray(std::vector<uint32_t>& indices) const {
        std::vector<Point3<float>> vertexArray;
        vertexArray.reserve((2ul * sides) + 2ul);

        float radius = cylinder.getRadius();
        float halfHeight = cylinder.getHeight() / 2.0f;
        float angle = (2.0f * MathValue::PI_FLOAT) / (float)sides;

        Quaternion<float> qCylinderOrientation;
        CylinderShape<float>::CylinderOrientation cylinderOrientation = cylinder.getCylinderOrientation();
        if (cylinderOrientation == CylinderShape<float>::CYLINDER_X) {
            qCylinderOrientation = Quaternion<float>::rotationY(MathValue::PI_FLOAT / 2.0f);
        } else if (cylinderOrientation == CylinderShape<float>::CYLINDER_Y) {
            qCylinderOrientation = Quaternion<float>::rotationX(MathValue::PI_FLOAT / 2.0f);
        } else if (cylinderOrientation == CylinderShape<float>::CYLINDER_Z) {
            qCylinderOrientation = Quaternion<float>(0.0f, 0.0f, 0.0f, 1.0f);
        }

        Quaternion<float> localOrientation = cylinder.getOrientation() * qCylinderOrientation;
        for (unsigned int i = 0; i < sides; i++) {
            float x1 = std::cos((float)i * angle) * radius;
            float y1 = std::sin((float)i * angle) * radius;
            vertexArray.push_back(cylinder.getCenterOfMass() + localOrientation.rotatePoint(Point3<float>(x1, y1, halfHeight)));
            vertexArray.push_back(cylinder.getCenterOfMass() + localOrientation.rotatePoint(Point3<float>(x1, y1, -halfHeight)));
        }
        vertexArray.push_back(cylinder.getCenterOfMass() + localOrientation.rotatePoint(Point3<float>(0.0f, 0.0f, halfHeight)));
        vertexArray.push_back(cylinder.getCenterOfMass() + localOrientation.rotatePoint(Point3<float>(0.0f, 0.0f, -halfHeight)));

        indices.reserve(6ul * sides);
        for (uint32_t i = 0; i < sides; i++) {
            uint32_t leftPos = i * 2;
            uint32_t leftNeg = i * 2 + 1;

            bool isLast = i == sides - 1;
            uint32_t rightPos = isLast ? 0 : (i + 1) * 2;
            uint32_t rightNeg = isLast ? 1 : (i + 1) * 2 + 1;

            //cylinder
            indices.push_back(leftPos);
            indices.push_back(rightNeg);
            indices.push_back(leftNeg);

            indices.push_back(leftPos);
            indices.push_back(rightPos);
            indices.push_back(rightNeg);

            //caps
            indices.push_back(rightNeg);
            indices.push_back((uint32_t)vertexArray.size() - 1);
            indices.push_back(leftNeg);

            indices.push_back(leftPos);
            indices.push_back((uint32_t)vertexArray.size() - 2);
            indices.push_back(rightPos);
        }

        return vertexArray;
    }

    ShapeType CylinderModel::getShapeType() const {
        return ShapeType::TRIANGLE;
    }

}
