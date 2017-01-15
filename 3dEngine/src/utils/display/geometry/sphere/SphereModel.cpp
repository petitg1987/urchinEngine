#include <GL/glew.h>
#include <GL/gl.h>
#include <cmath>

#include "utils/display/geometry/sphere/SphereModel.h"

namespace urchin
{

	SphereModel::SphereModel(const Sphere<float> &sphere, int slices):
			sphere(sphere),
			slices(slices)
	{
		initialize();
	}

	SphereModel::~SphereModel()
	{

	}

	Matrix4<float> SphereModel::retrieveModelMatrix() const
	{
		Matrix4<float> modelMatrix;
		modelMatrix.buildTranslation(sphere.getCenterOfMass().X, sphere.getCenterOfMass().Y, sphere.getCenterOfMass().Z);

		return modelMatrix;
	}

	std::vector<Point3<float>> SphereModel::retrieveVertexArray() const
	{
		std::vector<Point3<float>> vertexArray;
		vertexArray.reserve(2*slices*(slices+1));

		float radius = sphere.getRadius();

		for(int i=1; i<=slices; i++)
		{
			float latitude0 = PI_VALUE * (-0.5 + (float) (i - 1) / slices);
			float z0 = std::sin(latitude0);
			float zr0 = std::cos(latitude0);

			float latitude1 = PI_VALUE * (-0.5 + (float) i / slices);
			float z1 = std::sin(latitude1);
			float zr1 = std::cos(latitude1);

			for(int j=0; j<=slices; j++)
			{
				float longitude = 2 * PI_VALUE * (float) (j - 1) / slices;
				float x = std::cos(longitude);
				float y = std::sin(longitude);

				vertexArray.push_back(Point3<float>(x*zr0*radius, y*zr0*radius, z0*radius));
				vertexArray.push_back(Point3<float>(x*zr1*radius, y*zr1*radius, z1*radius));
			}
		}

		return vertexArray;
	}

	void SphereModel::drawGeometry() const
	{
		glDrawArrays(GL_QUAD_STRIP, 0, 2*slices*(slices+1));
	}
}
