#include <GL/glew.h>
#include <GL/gl.h>

#include "LinesModel.h"

namespace urchin
{

	LinesModel::LinesModel(const std::vector<Point3<float>> &linesPoints, int linesSize) :
			linesPoints(linesPoints),
			linesSize(linesSize)
	{
		initialize();
	}

    LinesModel::LinesModel(const Line3D<float> &line, int linesSize) :
        linesSize(linesSize)
    {
        linesPoints.push_back(line.getA());
        linesPoints.push_back(line.getB());

        initialize();
    }

	Matrix4<float> LinesModel::retrieveModelMatrix() const
	{
		return {};
	}

	std::vector<Point3<float>> LinesModel::retrieveVertexArray() const
	{
		return linesPoints;
	}

	void LinesModel::drawGeometry() const
	{
		GLfloat savedLineWidth;
		glGetFloatv(GL_LINE_WIDTH, &savedLineWidth);
		glLineWidth(linesSize);

		glDrawArrays(GL_LINE_STRIP, 0, linesPoints.size());

		glLineWidth(savedLineWidth);
	}
}
