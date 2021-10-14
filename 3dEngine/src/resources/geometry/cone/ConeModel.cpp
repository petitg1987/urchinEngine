#include <cmath>
#include <utility>

#include <resources/geometry/cone/ConeModel.h>

namespace urchin {

    ConeModel::ConeModel(Cone<float> cone, unsigned int slices):
            cone(std::move(cone)),
            slices(slices) {

    }

    std::vector<Point3<float>> ConeModel::retrieveVertexArray(std::vector<uint32_t>& indices) const {
        indices.reserve(6 * slices);
        std::vector<Point3<float>> vertexArray;
        vertexArray.reserve(slices + 2);

        float radius = cone.getRadius();
        float angle = (2.0f * MathValue::PI_FLOAT) / (float)slices;

        Quaternion<float> qConeOrientation;
        ConeShape<float>::ConeOrientation coneOrientation = cone.getConeOrientation();
        if (coneOrientation == ConeShape<float>::CONE_X_POSITIVE) {
            qConeOrientation = Quaternion<float>::rotationZ(-MathValue::PI_FLOAT / 2.0f);
        } else if (coneOrientation == ConeShape<float>::CONE_X_NEGATIVE) {
            qConeOrientation = Quaternion<float>::rotationZ(MathValue::PI_FLOAT / 2.0f);
        } else if (coneOrientation == ConeShape<float>::CONE_Y_POSITIVE) {
            qConeOrientation = Quaternion<float>(0.0f, 0.0f, 0.0f, 1.0f);
        } else if (coneOrientation == ConeShape<float>::CONE_Y_NEGATIVE) {
            qConeOrientation = Quaternion<float>::rotationZ(MathValue::PI_FLOAT);
        } else if (coneOrientation == ConeShape<float>::CONE_Z_POSITIVE) {
            qConeOrientation = Quaternion<float>::rotationX(MathValue::PI_FLOAT / 2.0f);
        } else if (coneOrientation == ConeShape<float>::CONE_Z_NEGATIVE) {
            qConeOrientation = Quaternion<float>::rotationX(-MathValue::PI_FLOAT / 2.0f);
        }

        Quaternion<float> localOrientation = cone.getOrientation() * qConeOrientation;
        Point3<float> topPoint = localOrientation.rotatePoint(Point3<float>(0.0f, cone.getHeight() * (3.0f / 4.0f), 0.0f));

        for (unsigned int i = 0; i < slices; i++) {
            float x1 = std::cos((float)i * angle) * radius;
            float z1 = std::sin((float)i * angle) * radius;
            vertexArray.push_back(cone.getCenterOfMass() + localOrientation.rotatePoint(Point3<float>(x1, -cone.getHeight() * (1.0f / 4.0f), z1)));
        }
        vertexArray.push_back(cone.getCenterOfMass() + topPoint);
        vertexArray.push_back(cone.getCenterOfMass() + localOrientation.rotatePoint(Point3<float>(0.0f, -cone.getHeight() * (1.0f / 4.0f), 0.0f)));

        for (unsigned int i = 0; i < slices; i++) {
            uint32_t baseIndex = i;
            uint32_t nextBaseIndex = (i == slices - 1) ? 0 : i + 1;

            //hull
            indices.push_back((uint32_t)vertexArray.size() - 2);
            indices.push_back(baseIndex);
            indices.push_back(nextBaseIndex);

            //base cap
            indices.push_back(baseIndex);
            indices.push_back((uint32_t)vertexArray.size() - 1);
            indices.push_back(nextBaseIndex);
        }

        return vertexArray;
    }

    ShapeType ConeModel::getShapeType() const {
        return ShapeType::TRIANGLE;
    }

}
