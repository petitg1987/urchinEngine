#include <GL/glew.h>
#include <cmath>
#include <utility>

#include "utils/display/geometry/cone/ConeModel.h"

namespace urchin
{

    ConeModel::ConeModel(Cone<float> cone, int slices):
            cone(std::move(cone)),
            slices(slices)
    {
        initialize();
    }

    Matrix4<float> ConeModel::retrieveModelMatrix() const
    {
        Matrix4<float> modelMatrix;
        modelMatrix.buildTranslation(cone.getCenterOfMass().X, cone.getCenterOfMass().Y, cone.getCenterOfMass().Z);

        return modelMatrix;
    }

    std::vector<Point3<float>> ConeModel::retrieveVertexArray() const
    {
        std::vector<Point3<float>> vertexArray;
        vertexArray.reserve(1+(slices+1));

        float radius = cone.getRadius();
        float angle = (2.0f * (float)PI_VALUE) / (float)slices;

        Quaternion<float> qConeOrientation;
        ConeShape<float>::ConeOrientation coneOrientation = cone.getConeOrientation();
        if(coneOrientation==ConeShape<float>::CONE_X_POSITIVE)
        {
            qConeOrientation = Quaternion<float>(Vector3<float>(0.0, 0.0, -1.0), PI_VALUE/2.0);
        }else if(coneOrientation==ConeShape<float>::CONE_X_NEGATIVE)
        {
            qConeOrientation = Quaternion<float>(Vector3<float>(0.0, 0.0, 1.0), PI_VALUE/2.0);
        }else if(coneOrientation==ConeShape<float>::CONE_Y_POSITIVE)
        {
            qConeOrientation = Quaternion<float>(0.0, 0.0, 0.0, 1.0);
        }else if(coneOrientation==ConeShape<float>::CONE_Y_NEGATIVE)
        {
            qConeOrientation = Quaternion<float>(Vector3<float>(0.0, 0.0, 1.0), PI_VALUE);
        }else if(coneOrientation==ConeShape<float>::CONE_Z_POSITIVE)
        {
            qConeOrientation = Quaternion<float>(Vector3<float>(1.0, 0.0, 0.0), PI_VALUE/2.0);
        }else if(coneOrientation==ConeShape<float>::CONE_Z_NEGATIVE)
        {
            qConeOrientation = Quaternion<float>(Vector3<float>(-1.0, 0.0, 0.0), PI_VALUE/2.0);
        }

        Quaternion<float> localOrientation = cone.getOrientation() * qConeOrientation;
        Point3<float> topPoint = localOrientation.rotatePoint(Point3<float>(0.0, cone.getHeight()*(3.0/4.0), 0.0));
        vertexArray.push_back(topPoint);
        for (int i = 0; i <= slices; i++)
        {
            float x = std::cos((float)i * angle) * radius;
            float z = std::sin((float)i * angle) * radius;

            Point3<float> bottomPoint = localOrientation.rotatePoint(Point3<float>(x, -cone.getHeight()*(1.0/4.0), z));
            vertexArray.push_back(bottomPoint);
        }

        return vertexArray;
    }

    void ConeModel::drawGeometry() const
    {
        glDrawArrays(GL_TRIANGLE_FAN, 0, 1 + (slices + 1));
    }

}
