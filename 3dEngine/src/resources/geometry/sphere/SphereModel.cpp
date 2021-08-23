#include <cmath>
#include <utility>

#include <resources/geometry/sphere/SphereModel.h>

namespace urchin {

    SphereModel::SphereModel(Sphere<float> sphere, unsigned int slices):
            slices(slices) {
        spheres.push_back(std::move(sphere));
    }

    SphereModel::SphereModel(std::vector<Sphere<float>> spheres, unsigned int slices):
            spheres(std::move(spheres)),
            slices(slices) {

    }

    std::vector<Point3<float>> SphereModel::retrieveVertexArray() const {
        std::vector<Point3<float>> vertexArray;
        vertexArray.reserve(spheres.size() * 6 * slices * slices);

        for(const Sphere<float>& sphere : spheres) {
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

                    vertexArray.emplace_back(sphere.getCenterOfMass() + Point3<float>(x1 * zr0 * radius, y1 * zr0 * radius, z0 * radius));
                    vertexArray.emplace_back(sphere.getCenterOfMass() + Point3<float>(x1 * zr1 * radius, y1 * zr1 * radius, z1 * radius));
                    vertexArray.emplace_back(sphere.getCenterOfMass() + Point3<float>(x2 * zr1 * radius, y2 * zr1 * radius, z1 * radius));

                    vertexArray.emplace_back(sphere.getCenterOfMass() + Point3<float>(x1 * zr0 * radius, y1 * zr0 * radius, z0 * radius));
                    vertexArray.emplace_back(sphere.getCenterOfMass() + Point3<float>(x2 * zr1 * radius, y2 * zr1 * radius, z1 * radius));
                    vertexArray.emplace_back(sphere.getCenterOfMass() + Point3<float>(x2 * zr0 * radius, y2 * zr0 * radius, z0 * radius));
                }
            }
        }

        return vertexArray;
    }

    ShapeType SphereModel::getShapeType() const {
        return ShapeType::TRIANGLE;
    }
}
