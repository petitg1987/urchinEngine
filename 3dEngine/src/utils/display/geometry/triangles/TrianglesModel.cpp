#include <GL/glew.h>
#include <GL/gl.h>

#include "TrianglesModel.h"

namespace urchin
{

	TrianglesModel::TrianglesModel(const std::vector<Point3<float>> &points, const std::vector<IndexedTriangle2D<float>> &triangles):
			points(points),
			triangles(triangles)
	{
		initialize();
	}

	TrianglesModel::~TrianglesModel()
	{

	}

	Matrix4<float> TrianglesModel::retrieveModelMatrix() const
	{
		return Matrix4<float>();
	}

	std::vector<Point3<float>> TrianglesModel::retrieveVertexArray() const
	{
		std::vector<Point3<float>> vertexArray;

		for(const auto &triangle : triangles)
		{
			vertexArray.push_back(points[triangle.getIndexes()[0]]);
			vertexArray.push_back(points[triangle.getIndexes()[1]]);
			vertexArray.push_back(points[triangle.getIndexes()[2]]);
		}

		return vertexArray;
	}

	void TrianglesModel::drawGeometry() const
	{
		//TODO change it
		glLineWidth(4.0);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDrawArrays(GL_TRIANGLES, 0, triangles.size()*3);

		glDisable(GL_BLEND);
	}

}
