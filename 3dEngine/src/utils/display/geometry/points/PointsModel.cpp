#include <GL/gl.h>

#include "utils/display/geometry/points/PointsModel.h"

namespace urchin
{

	PointsModel::PointsModel(const std::vector<Point3<float>> &points, int pointsSize):
			points(points),
			pointsSize(pointsSize)
	{
		initialize();
	}

	PointsModel::~PointsModel()
	{

	}

	Matrix4<float> PointsModel::retrieveModelMatrix() const
	{
		return Matrix4<float>();
	}

	std::vector<Point3<float>> PointsModel::retrieveVertexArray() const
	{
		return points;
	}

	void PointsModel::drawGeometry() const
	{
		GLfloat savedPointSize;
		glGetFloatv(GL_POINT_SIZE, &savedPointSize);
		glPointSize(pointsSize);

		glDrawArrays(GL_POINTS, 0, points.size());

		glPointSize(savedPointSize);
	}
}
