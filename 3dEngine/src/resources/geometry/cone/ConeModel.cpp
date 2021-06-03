#include <cmath>
#include <utility>

#include <resources/geometry/cone/ConeModel.h>

namespace urchin {

    ConeModel::ConeModel(Cone<float> cone, unsigned int slices):
            cone(std::move(cone)),
            slices(slices) {

    }

    Matrix4<float> ConeModel::retrieveModelMatrix() const {
        Matrix4<float> modelMatrix;
        modelMatrix.buildTranslation(cone.getCenterOfMass().X, cone.getCenterOfMass().Y, cone.getCenterOfMass().Z);

        return modelMatrix;
    }

    std::vector<Point3<float>> ConeModel::retrieveVertexArray() const {
        std::vector<Point3<float>> vertexArray;
        vertexArray.reserve(1 + (slices + 1));

        float radius = cone.getRadius();
        float angle = (2.0f * MathValue::PI_FLOAT) / (float)slices;

        Quaternion<float> qConeOrientation;
        ConeShape<float>::ConeOrientation coneOrientation = cone.getConeOrientation();
        if (coneOrientation == ConeShape<float>::CONE_X_POSITIVE) {
            qConeOrientation = Quaternion<float>(Vector3<float>(0.0f, 0.0f, -1.0f), MathValue::PI_FLOAT / 2.0f);
        } else if (coneOrientation == ConeShape<float>::CONE_X_NEGATIVE) {
            qConeOrientation = Quaternion<float>(Vector3<float>(0.0f, 0.0f, 1.0f), MathValue::PI_FLOAT / 2.0f);
        } else if (coneOrientation == ConeShape<float>::CONE_Y_POSITIVE) {
            qConeOrientation = Quaternion<float>(0.0f, 0.0f, 0.0f, 1.0);
        } else if (coneOrientation == ConeShape<float>::CONE_Y_NEGATIVE) {
            qConeOrientation = Quaternion<float>(Vector3<float>(0.0f, 0.0f, 1.0f), MathValue::PI_FLOAT);
        } else if (coneOrientation == ConeShape<float>::CONE_Z_POSITIVE) {
            qConeOrientation = Quaternion<float>(Vector3<float>(1.0f, 0.0f, 0.0f), MathValue::PI_FLOAT / 2.0f);
        } else if (coneOrientation == ConeShape<float>::CONE_Z_NEGATIVE) {
            qConeOrientation = Quaternion<float>(Vector3<float>(-1.0f, 0.0f, 0.0f), MathValue::PI_FLOAT / 2.0f);
        }

        Quaternion<float> localOrientation = cone.getOrientation() * qConeOrientation;
        Point3<float> topPoint = localOrientation.rotatePoint(Point3<float>(0.0f, cone.getHeight() * (3.0f / 4.0f), 0.0f));

        for (unsigned int i = 0; i < slices; i++) {
            float x1 = std::cos((float)i * angle) * radius;
            float z1 = std::sin((float)i * angle) * radius;
            float x2 = std::cos((float)i * angle) * radius;
            float z2 = std::sin((float)i * angle) * radius;

            vertexArray.push_back(topPoint);
            vertexArray.push_back(localOrientation.rotatePoint(Point3<float>(x1, -cone.getHeight() * (1.0f / 4.0f), z1)));
            vertexArray.push_back(localOrientation.rotatePoint(Point3<float>(x2, -cone.getHeight() * (1.0f / 4.0f), z2)));
        }

        return vertexArray;
    }

    ShapeType ConeModel::getShapeType() const {
        return ShapeType::TRIANGLE;
    }

}
