#include <GL/glew.h>
#include <GL/gl.h>

#include "utils/display/geometry/line/LineModel.h"

namespace urchin
{

	LineModel::LineModel(const Line3D<float> &line, int lineSize):
		line(line),
		lineSize(lineSize)
	{
		initialize();
	}

	Matrix4<float> LineModel::retrieveModelMatrix() const
	{
		return {};
	}

	std::vector<Point3<float>> LineModel::retrieveVertexArray() const
	{
		std::vector<Point3<float>> vertexArray;
		vertexArray.reserve(2);

		vertexArray.push_back(line.getA());
		vertexArray.push_back(line.getB());

		return vertexArray;
	}

	void LineModel::drawGeometry() const
	{
		GLfloat savedLineWidth;
		glGetFloatv(GL_LINE_WIDTH, &savedLineWidth);
		glLineWidth(lineSize);

		glDrawArrays(GL_LINES, 0, 2);

		glLineWidth(savedLineWidth);
	}
}
