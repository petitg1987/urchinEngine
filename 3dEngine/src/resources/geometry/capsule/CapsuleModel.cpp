#include <cmath>
#include <utility>

#include "resources/geometry/capsule/CapsuleModel.h"

namespace urchin {

    CapsuleModel::CapsuleModel(Capsule<float> capsule, int sides, int slices):
            capsule(std::move(capsule)),
            sides(sides) {

        //number of slices must be an even number to have same number of faces for the bottom and the top caps
        this->slices = (slices%2==0) ? slices : slices + 1;

        initialize();
    }

    Matrix4<float> CapsuleModel::retrieveModelMatrix() const {
        Matrix4<float> modelMatrix;
        modelMatrix.buildTranslation(capsule.getCenterOfMass().X, capsule.getCenterOfMass().Y, capsule.getCenterOfMass().Z);

        return modelMatrix;
    }

    std::vector<Point3<float>> CapsuleModel::retrieveVertexArray() const {
        std::vector<Point3<float>> vertexArray;
        vertexArray.reserve(4*sides + 2*(4*(slices/2)*slices));

        float radius = capsule.getRadius();
        float halfCylinderHeight = capsule.getCylinderHeight() / 2.0f;
        float angle = (2.0f * (float)PI_VALUE) / (float)sides;

        CapsuleShape<float>::CapsuleOrientation capsuleOrientation = capsule.getCapsuleOrientation();
        Quaternion<float> qCapsuleOrientation, qCapOrientation;
        if (capsuleOrientation == CapsuleShape<float>::CAPSULE_X) {
            qCapsuleOrientation = Quaternion<float>(Vector3<float>(0.0, 1.0, 0.0), PI_VALUE/2.0);
            qCapOrientation = Quaternion<float>(Vector3<float>(0.0, 0.0, 1.0), PI_VALUE/2.0);
        } else if (capsuleOrientation == CapsuleShape<float>::CAPSULE_Y) {
            qCapsuleOrientation = Quaternion<float>(Vector3<float>(1.0, 0.0, 0.0), PI_VALUE/2.0);
            qCapOrientation = Quaternion<float>(0.0, 0.0, 0.0, 1.0);
        } else if (capsuleOrientation == CapsuleShape<float>::CAPSULE_Z) {
            qCapsuleOrientation = Quaternion<float>(0.0, 0.0, 0.0, 1.0);
            qCapOrientation = Quaternion<float>(Vector3<float>(1.0, 0.0, 0.0), PI_VALUE/2.0);
        }

        //cylinder
        Quaternion<float> localCylinderOrientation = capsule.getOrientation() * qCapsuleOrientation;
        for (int i = 0; i < sides; i++) {
            float x1 = std::cos((float)i * angle) * radius;
            float y1 = std::sin((float)i * angle) * radius;
            float x2 = std::cos((float)(i+1) * angle) * radius;
            float y2 = std::sin((float)(i+1) * angle) * radius;

            vertexArray.push_back(localCylinderOrientation.rotatePoint(Point3<float>(x1, y1, halfCylinderHeight)));
            vertexArray.push_back(localCylinderOrientation.rotatePoint(Point3<float>(x1, y1, -halfCylinderHeight)));
            vertexArray.push_back(localCylinderOrientation.rotatePoint(Point3<float>(x2, y2, -halfCylinderHeight)));

            vertexArray.push_back(localCylinderOrientation.rotatePoint(Point3<float>(x1, y1, halfCylinderHeight)));
            vertexArray.push_back(localCylinderOrientation.rotatePoint(Point3<float>(x2, y2, -halfCylinderHeight)));
            vertexArray.push_back(localCylinderOrientation.rotatePoint(Point3<float>(x2, y2, halfCylinderHeight)));
        }

        //caps
        Quaternion<float> localCapOrientation = capsule.getOrientation() * qCapOrientation;
        fillWithCaps(vertexArray, localCapOrientation);

        return vertexArray;
    }

    void CapsuleModel::fillWithCaps(std::vector<Point3<float>> &vertexArray, const Quaternion<float> &localCapOrientation) const {
        float radius = capsule.getRadius();
        float halfCylinderHeight = capsule.getCylinderHeight() / 2.0f;

        int nbLong = slices/2;
        for (int i = 1; i <= slices; i++) {
            float latitude1 = PI_VALUE * (-0.5 + (float)(i - 1) / (float)slices);
            float z0 = std::sin(latitude1);
            float zr0 = std::cos(latitude1);

            float latitude2 = PI_VALUE * (-0.5 + (float)i / (float)slices);
            float z1 = std::sin(latitude2);
            float zr1 = std::cos(latitude2);

            //bottom cap
            for (int j = 0; j < nbLong; j++) {
                float longitude1 = PI_VALUE * (float)(j) / nbLong;
                float longitude2 = PI_VALUE * (float)(j+1) / nbLong;
                float x1 = std::cos(longitude1);
                float y1 = std::sin(longitude1);
                float x2 = std::cos(longitude2);
                float y2 = std::sin(longitude2);

                vertexArray.push_back(localCapOrientation.rotatePoint(Point3<float>(x1*zr0*radius, y1*zr0*radius+halfCylinderHeight, z0*radius)));
                vertexArray.push_back(localCapOrientation.rotatePoint(Point3<float>(x1*zr1*radius, y1*zr1*radius+halfCylinderHeight, z1*radius)));
                vertexArray.push_back(localCapOrientation.rotatePoint(Point3<float>(x2*zr1*radius, y2*zr1*radius+halfCylinderHeight, z1*radius)));

                vertexArray.push_back(localCapOrientation.rotatePoint(Point3<float>(x1*zr0*radius, y1*zr0*radius+halfCylinderHeight, z0*radius)));
                vertexArray.push_back(localCapOrientation.rotatePoint(Point3<float>(x2*zr1*radius, y2*zr1*radius+halfCylinderHeight, z1*radius)));
                vertexArray.push_back(localCapOrientation.rotatePoint(Point3<float>(x2*zr0*radius, y2*zr0*radius+halfCylinderHeight, z0*radius)));
            }

            //top cap
            for (int j = nbLong; j < slices; j++) {
                float longitude1 = PI_VALUE * (float)(j) / nbLong;
                float longitude2 = PI_VALUE * (float)(j+1) / nbLong;
                float x1 = std::cos(longitude1);
                float y1 = std::sin(longitude1);
                float x2 = std::cos(longitude2);
                float y2 = std::sin(longitude2);

                vertexArray.push_back(localCapOrientation.rotatePoint(Point3<float>(x1*zr0*radius, y1*zr0*radius-halfCylinderHeight, z0*radius)));
                vertexArray.push_back(localCapOrientation.rotatePoint(Point3<float>(x1*zr1*radius, y1*zr1*radius-halfCylinderHeight, z1*radius)));
                vertexArray.push_back(localCapOrientation.rotatePoint(Point3<float>(x2*zr1*radius, y2*zr1*radius-halfCylinderHeight, z1*radius)));

                vertexArray.push_back(localCapOrientation.rotatePoint(Point3<float>(x1*zr0*radius, y1*zr0*radius-halfCylinderHeight, z0*radius)));
                vertexArray.push_back(localCapOrientation.rotatePoint(Point3<float>(x2*zr1*radius, y2*zr1*radius-halfCylinderHeight, z1*radius)));
                vertexArray.push_back(localCapOrientation.rotatePoint(Point3<float>(x2*zr0*radius, y2*zr0*radius-halfCylinderHeight, z0*radius)));
            }
        }
    }

    ShapeType CapsuleModel::getShapeType() const {
        return ShapeType::TRIANGLE;
    }

}
