#include <GL/glew.h>
#include <cmath>
#include <utility>

#include "utils/display/geometry/sphere/SphereModel.h"

namespace urchin {

    SphereModel::SphereModel(Sphere<float> sphere, int slices):
            sphere(std::move(sphere)),
            slices(slices) {
        initialize();
    }

    Matrix4<float> SphereModel::retrieveModelMatrix() const {
        Matrix4<float> modelMatrix;
        modelMatrix.buildTranslation(sphere.getCenterOfMass().X, sphere.getCenterOfMass().Y, sphere.getCenterOfMass().Z);

        return modelMatrix;
    }

    std::vector<Point3<float>> SphereModel::retrieveVertexArray() const {
        std::vector<Point3<float>> vertexArray;
        vertexArray.reserve(2*slices*(slices+1));

        float radius = sphere.getRadius();

        for (int i=1; i<=slices; i++) {
            float latitude0 = PI_VALUE * (-0.5f + (float)(i - 1) / (float)slices);
            float z0 = std::sin(latitude0);
            float zr0 = std::cos(latitude0);

            float latitude1 = PI_VALUE * (-0.5f + (float)i / (float)slices);
            float z1 = std::sin(latitude1);
            float zr1 = std::cos(latitude1);

            for (int j=0; j<=slices; j++) {
                float longitude = 2.0f * (float)PI_VALUE * (float)(j - 1) / (float)slices;
                float x = std::cos(longitude);
                float y = std::sin(longitude);

                vertexArray.emplace_back(Point3<float>(x*zr0*radius, y*zr0*radius, z0*radius));
                vertexArray.emplace_back(Point3<float>(x*zr1*radius, y*zr1*radius, z1*radius));
            }
        }

        return vertexArray;
    }

    void SphereModel::drawGeometry() const {
        glDrawArrays(GL_QUAD_STRIP, 0, 2*slices*(slices+1));
    }
}
