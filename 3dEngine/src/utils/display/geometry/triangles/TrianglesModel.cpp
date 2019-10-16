#include <GL/glew.h>

#include <utility>

#include "TrianglesModel.h"

namespace urchin
{

	TrianglesModel::TrianglesModel(std::vector<Point3<float>> points, std::vector<IndexedTriangle3D<float>> triangles):
			points(std::move(points)),
			triangles(std::move(triangles))
	{
		initialize();
	}

	Matrix4<float> TrianglesModel::retrieveModelMatrix() const
	{
		return {};
	}

	std::vector<Point3<float>> TrianglesModel::retrieveVertexArray() const
	{
		std::vector<Point3<float>> vertexArray;
		vertexArray.reserve(triangles.size()*3);

		for(const auto &triangle : triangles)
		{
			vertexArray.push_back(points[triangle.getIndex(0)]);
			vertexArray.push_back(points[triangle.getIndex(1)]);
			vertexArray.push_back(points[triangle.getIndex(2)]);
		}

		return vertexArray;
	}

	void TrianglesModel::drawGeometry() const
	{
		glDrawArrays(GL_TRIANGLES, 0, triangles.size()*3);
	}

}
