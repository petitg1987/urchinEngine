#include <GL/glew.h>
#include <GL/gl.h>
#include <cmath>

#include "utils/display/geometry/capsule/CapsuleModel.h"

namespace urchin
{

	CapsuleModel::CapsuleModel(const Capsule<float> &capsule, int sides, int slices):
			capsule(capsule),
			sides(sides),
			slices(slices)
	{
		initialize();
	}

	CapsuleModel::~CapsuleModel()
	{

	}

	Matrix4<float> CapsuleModel::retrieveModelMatrix() const
	{
		Matrix4<float> modelMatrix;
		modelMatrix.buildTranslation(capsule.getCenterPosition().X, capsule.getCenterPosition().Y, capsule.getCenterPosition().Z);

		return modelMatrix;
	}

	std::vector<Point3<float>> CapsuleModel::retrieveVertexArray() const
	{
		std::vector<Point3<float>> vertexArray;
		vertexArray.reserve(2*(sides+1) + 2*(slices)*(slices+1));

		float radius = capsule.getRadius();
		float halfCylinderHeight = capsule.getCylinderHeight() / 2.0;
		float angle = (2.0*PI_VALUE) / sides;

		CapsuleShape<float>::CapsuleOrientation capsuleOrientation = capsule.getCapsuleOrientation();
		Quaternion<float> qCapsuleOrientation, qCapOrientation;
		if(capsuleOrientation==CapsuleShape<float>::CAPSULE_X)
		{
			qCapsuleOrientation = Quaternion<float>(Vector3<float>(0.0, 1.0, 0.0), PI_VALUE/2.0);
			qCapOrientation = Quaternion<float>(Vector3<float>(0.0, 0.0, 1.0), PI_VALUE/2.0);
		}else if(capsuleOrientation==CapsuleShape<float>::CAPSULE_Y)
		{
			qCapsuleOrientation = Quaternion<float>(Vector3<float>(1.0, 0.0, 0.0), PI_VALUE/2.0);
			qCapOrientation = Quaternion<float>(0.0, 0.0, 0.0, 1.0);
		}else if(capsuleOrientation==CapsuleShape<float>::CAPSULE_Z)
		{
			qCapsuleOrientation = Quaternion<float>(0.0, 0.0, 0.0, 1.0);
			qCapOrientation = Quaternion<float>(Vector3<float>(1.0, 0.0, 0.0), PI_VALUE/2.0);
		}

		//cylinder
		Quaternion<float> localCylinderOrientation = capsule.getOrientation() * qCapsuleOrientation;
		for (int i = 0; i <= sides; i++) {
		    float x = std::cos(i*angle) * radius;
		    float y = std::sin(i*angle) * radius;

		    Point3<float> p1 = localCylinderOrientation.rotatePoint(Point3<float>(x, y, halfCylinderHeight));
		    Point3<float> p2 = localCylinderOrientation.rotatePoint(Point3<float>(x, y, -halfCylinderHeight));

		    vertexArray.push_back(p1);
		    vertexArray.push_back(p2);
		}

		//caps
		Quaternion<float> localCapOrientation = capsule.getOrientation() * qCapOrientation;
		fillWithTopCap(vertexArray, localCapOrientation);
		fillWithBottomCap(vertexArray, localCapOrientation);

		return vertexArray;
	}

	void CapsuleModel::fillWithTopCap(std::vector<Point3<float>> &vertexArray, const Quaternion<float> &localCapOrientation) const
	{
		float radius = capsule.getRadius();
		float halfCylinderHeight = capsule.getCylinderHeight() / 2.0f;

		int nbLong = slices/2;
		for(int i=1; i<=slices; i++)
		{
			float latitude0 = PI_VALUE * (-0.5 + (float) (i - 1) / slices);
			float z0 = std::sin(latitude0);
			float zr0 = std::cos(latitude0);

			float latitude1 = PI_VALUE * (-0.5 + (float) i / slices);
			float z1 = std::sin(latitude1);
			float zr1 = std::cos(latitude1);

			for(int j=0; j<=nbLong; j++)
			{
				float lng = PI_VALUE * (float)(j) / nbLong;
				float x = std::cos(lng);
				float y = std::sin(lng);

				Point3<float> p1 = localCapOrientation.rotatePoint(Point3<float>(x*zr0*radius, y*zr0*radius+halfCylinderHeight, z0*radius));
				Point3<float> p2 = localCapOrientation.rotatePoint(Point3<float>(x*zr1*radius, y*zr1*radius+halfCylinderHeight, z1*radius));

				vertexArray.push_back(p1);
				vertexArray.push_back(p2);
			}
		}
	}

	void CapsuleModel::fillWithBottomCap(std::vector<Point3<float>> &vertexArray, const Quaternion<float> &localCapOrientation) const
	{
		float radius = capsule.getRadius();
		float halfCylinderHeight = capsule.getCylinderHeight() / 2.0f;

		int nbLong = slices/2;
		for(int i=1; i<=slices; i++)
		{
			float latitude0 = PI_VALUE * (-0.5 + (float) (i - 1) / slices);
			float z0 = std::sin(latitude0);
			float zr0 = std::cos(latitude0);

			float latitude1 = PI_VALUE * (-0.5 + (float) i / slices);
			float z1 = std::sin(latitude1);
			float zr1 = std::cos(latitude1);

			for(int j=nbLong; j<slices; j++)
			{
				float longitude = PI_VALUE * (float)(j) / nbLong;
				float x = std::cos(longitude);
				float y = std::sin(longitude);

				Point3<float> p1 = localCapOrientation.rotatePoint(Point3<float>(x*zr0*radius, y*zr0*radius-halfCylinderHeight, z0*radius));
				Point3<float> p2 = localCapOrientation.rotatePoint(Point3<float>(x*zr1*radius, y*zr1*radius-halfCylinderHeight, z1*radius));

				vertexArray.push_back(p1);
				vertexArray.push_back(p2);
			}
		}
	}

	void CapsuleModel::drawGeometry() const
	{
		int cylinderCount = 2*(sides+1);
		glDrawArrays(GL_QUAD_STRIP, 0, cylinderCount);

		int nbLong = slices/2;
		int topCapCount = 2*(slices)*(nbLong+1);
		glDrawArrays(GL_QUAD_STRIP, cylinderCount, topCapCount);

		int bottomCapCount = 2*(slices)*(nbLong+1);
		glDrawArrays(GL_QUAD_STRIP, cylinderCount+topCapCount, bottomCapCount);
	}

}
