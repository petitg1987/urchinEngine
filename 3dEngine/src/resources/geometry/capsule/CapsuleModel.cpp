#include <cmath>
#include <utility>

#include <resources/geometry/capsule/CapsuleModel.h>

namespace urchin {

    CapsuleModel::CapsuleModel(Capsule<float> capsule, unsigned int sides, unsigned int slices):
            capsule(std::move(capsule)),
            sides(sides),
            slices((slices % 2 == 0) ? slices : slices + 1) /*number of slices must be an even number to have same number of faces for the bottom and the top caps */ {

    }

    std::vector<Point3<float>> CapsuleModel::retrieveVertexArray() const {
        std::vector<Point3<float>> vertexArray;
        vertexArray.reserve(6 * sides + 2 * (6 * (slices / 2) * slices));

        float radius = capsule.getRadius();
        float halfCylinderHeight = capsule.getCylinderHeight() / 2.0f;
        float angle = (2.0f * MathValue::PI_FLOAT) / (float)sides;

        CapsuleShape<float>::CapsuleOrientation capsuleOrientation = capsule.getCapsuleOrientation();
        Quaternion<float> qCapsuleOrientation, qCapOrientation;
        if (capsuleOrientation == CapsuleShape<float>::CAPSULE_X) {
            qCapsuleOrientation = Quaternion<float>::rotationY(MathValue::PI_FLOAT / 2.0f);
            qCapOrientation = Quaternion<float>::rotationZ(MathValue::PI_FLOAT / 2.0f);
        } else if (capsuleOrientation == CapsuleShape<float>::CAPSULE_Y) {
            qCapsuleOrientation = Quaternion<float>::rotationX(MathValue::PI_FLOAT / 2.0f);
            qCapOrientation = Quaternion<float>(0.0f, 0.0f, 0.0f, 1.0f);
        } else if (capsuleOrientation == CapsuleShape<float>::CAPSULE_Z) {
            qCapsuleOrientation = Quaternion<float>(0.0f, 0.0f, 0.0f, 1.0f);
            qCapOrientation = Quaternion<float>::rotationX(MathValue::PI_FLOAT / 2.0f);
        }

        //cylinder
        Quaternion<float> localCylinderOrientation = capsule.getOrientation() * qCapsuleOrientation;
        for (unsigned int i = 0; i < sides; i++) {
            float x1 = std::cos((float)i * angle) * radius;
            float y1 = std::sin((float)i * angle) * radius;
            float x2 = std::cos((float)(i + 1) * angle) * radius;
            float y2 = std::sin((float)(i + 1) * angle) * radius;

            vertexArray.push_back(capsule.getCenterOfMass() + localCylinderOrientation.rotatePoint(Point3<float>(x1, y1, halfCylinderHeight)));
            vertexArray.push_back(capsule.getCenterOfMass() + localCylinderOrientation.rotatePoint(Point3<float>(x2, y2, -halfCylinderHeight)));
            vertexArray.push_back(capsule.getCenterOfMass() + localCylinderOrientation.rotatePoint(Point3<float>(x1, y1, -halfCylinderHeight)));

            vertexArray.push_back(capsule.getCenterOfMass() + localCylinderOrientation.rotatePoint(Point3<float>(x1, y1, halfCylinderHeight)));
            vertexArray.push_back(capsule.getCenterOfMass() + localCylinderOrientation.rotatePoint(Point3<float>(x2, y2, halfCylinderHeight)));
            vertexArray.push_back(capsule.getCenterOfMass() + localCylinderOrientation.rotatePoint(Point3<float>(x2, y2, -halfCylinderHeight)));
        }

        //caps
        Quaternion<float> localCapOrientation = capsule.getOrientation() * qCapOrientation;
        fillWithCaps(vertexArray, localCapOrientation);

        return vertexArray;
    }

    void CapsuleModel::fillWithCaps(std::vector<Point3<float>>& vertexArray, const Quaternion<float>& localCapOrientation) const {
        float radius = capsule.getRadius();
        float halfCylinderHeight = capsule.getCylinderHeight() / 2.0f;

        unsigned int nbLong = slices / 2;
        for (unsigned int i = 1; i <= slices; i++) {
            float latitude1 = MathValue::PI_FLOAT * (-0.5f + (float)(i - 1) / (float)slices);
            float z0 = std::sin(latitude1);
            float zr0 = std::cos(latitude1);

            float latitude2 = MathValue::PI_FLOAT * (-0.5f + (float)i / (float)slices);
            float z1 = std::sin(latitude2);
            float zr1 = std::cos(latitude2);

            //bottom cap
            for (unsigned int j = 0; j < nbLong; j++) {
                float longitude1 = MathValue::PI_FLOAT * (float)j / (float)nbLong;
                float longitude2 = MathValue::PI_FLOAT * (float)(j + 1) / (float)nbLong;
                float x1 = std::cos(longitude1);
                float y1 = std::sin(longitude1);
                float x2 = std::cos(longitude2);
                float y2 = std::sin(longitude2);

                vertexArray.push_back(capsule.getCenterOfMass() + localCapOrientation.rotatePoint(Point3<float>(x1 * zr0 * radius, y1 * zr0 * radius + halfCylinderHeight, z0 * radius)));
                vertexArray.push_back(capsule.getCenterOfMass() + localCapOrientation.rotatePoint(Point3<float>(x1 * zr1 * radius, y1 * zr1 * radius + halfCylinderHeight, z1 * radius)));
                vertexArray.push_back(capsule.getCenterOfMass() + localCapOrientation.rotatePoint(Point3<float>(x2 * zr1 * radius, y2 * zr1 * radius + halfCylinderHeight, z1 * radius)));

                vertexArray.push_back(capsule.getCenterOfMass() + localCapOrientation.rotatePoint(Point3<float>(x1 * zr0 * radius, y1 * zr0 * radius + halfCylinderHeight, z0 * radius)));
                vertexArray.push_back(capsule.getCenterOfMass() + localCapOrientation.rotatePoint(Point3<float>(x2 * zr1 * radius, y2 * zr1 * radius + halfCylinderHeight, z1 * radius)));
                vertexArray.push_back(capsule.getCenterOfMass() + localCapOrientation.rotatePoint(Point3<float>(x2 * zr0 * radius, y2 * zr0 * radius + halfCylinderHeight, z0 * radius)));
            }

            //top cap
            for (unsigned int j = nbLong; j < slices; j++) {
                float longitude1 = MathValue::PI_FLOAT * (float)j / (float)nbLong;
                float longitude2 = MathValue::PI_FLOAT * (float)(j + 1) / (float)nbLong;
                float x1 = std::cos(longitude1);
                float y1 = std::sin(longitude1);
                float x2 = std::cos(longitude2);
                float y2 = std::sin(longitude2);

                vertexArray.push_back(capsule.getCenterOfMass() + localCapOrientation.rotatePoint(Point3<float>(x1 * zr0 * radius, y1 * zr0 * radius-halfCylinderHeight, z0 * radius)));
                vertexArray.push_back(capsule.getCenterOfMass() + localCapOrientation.rotatePoint(Point3<float>(x1 * zr1 * radius, y1 * zr1 * radius-halfCylinderHeight, z1 * radius)));
                vertexArray.push_back(capsule.getCenterOfMass() + localCapOrientation.rotatePoint(Point3<float>(x2 * zr1 * radius, y2 * zr1 * radius-halfCylinderHeight, z1 * radius)));

                vertexArray.push_back(capsule.getCenterOfMass() + localCapOrientation.rotatePoint(Point3<float>(x1 * zr0 * radius, y1 * zr0 * radius-halfCylinderHeight, z0 * radius)));
                vertexArray.push_back(capsule.getCenterOfMass() + localCapOrientation.rotatePoint(Point3<float>(x2 * zr1 * radius, y2 * zr1 * radius-halfCylinderHeight, z1 * radius)));
                vertexArray.push_back(capsule.getCenterOfMass() + localCapOrientation.rotatePoint(Point3<float>(x2 * zr0 * radius, y2 * zr0 * radius-halfCylinderHeight, z0 * radius)));
            }
        }
    }

    ShapeType CapsuleModel::getShapeType() const {
        return ShapeType::TRIANGLE;
    }

}
