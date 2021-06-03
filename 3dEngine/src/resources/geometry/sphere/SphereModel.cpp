#include <cmath>
#include <utility>

#include <resources/geometry/sphere/SphereModel.h>

namespace urchin {

    SphereModel::SphereModel(Sphere<float> sphere, unsigned int slices):
            sphere(std::move(sphere)),
            slices(slices) {

    }

    Matrix4<float> SphereModel::retrieveModelMatrix() const {
        Matrix4<float> modelMatrix;
        modelMatrix.buildTranslation(sphere.getCenterOfMass().X, sphere.getCenterOfMass().Y, sphere.getCenterOfMass().Z);

        return modelMatrix;
    }

    std::vector<Point3<float>> SphereModel::retrieveVertexArray() const {
        std::vector<Point3<float>> vertexArray;
        vertexArray.reserve(4 * slices * slices);

        float radius = sphere.getRadius();

        for (unsigned int i = 1; i <= slices; i++) {
            float latitude1 = MathValue::PI_FLOAT * (-0.5f + (float)(i - 1) / (float)slices);
            float z0 = std::sin(latitude1);
            float zr0 = std::cos(latitude1);

            float latitude2 = MathValue::PI_FLOAT * (-0.5f + (float)i / (float)slices);
            float z1 = std::sin(latitude2);
            float zr1 = std::cos(latitude2);

            for (unsigned int j = 0; j < slices; j++) {
                float longitude1 = 2.0f * MathValue::PI_FLOAT * (float)j / (float)slices;
                float longitude2 = 2.0f * MathValue::PI_FLOAT * (float)(j + 1) / (float)slices;
                float x1 = std::cos(longitude1);
                float y1 = std::sin(longitude1);
                float x2 = std::cos(longitude2);
                float y2 = std::sin(longitude2);

                vertexArray.emplace_back(Point3<float>(x1*zr0*radius, y1*zr0*radius, z0*radius));
                vertexArray.emplace_back(Point3<float>(x1*zr1*radius, y1*zr1*radius, z1*radius));
                vertexArray.emplace_back(Point3<float>(x2*zr1*radius, y2*zr1*radius, z1*radius));

                vertexArray.emplace_back(Point3<float>(x1*zr0*radius, y1*zr0*radius, z0*radius));
                vertexArray.emplace_back(Point3<float>(x2*zr1*radius, y2*zr1*radius, z1*radius));
                vertexArray.emplace_back(Point3<float>(x2*zr0*radius, y2*zr0*radius, z0*radius));
            }
        }

        return vertexArray;
    }

    ShapeType SphereModel::getShapeType() const {
        return ShapeType::TRIANGLE;
    }
}
