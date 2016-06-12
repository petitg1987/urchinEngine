#include <GL/glew.h>
#include <GL/gl.h>
#include <map>

#include "utils/display/geometry/convexhull/ConvexHullModel.h"

namespace urchin
{

	ConvexHullModel::ConvexHullModel(const ConvexHull3D<float> &convexHull):
			convexHull(convexHull)
	{
		initialize();
	}

	ConvexHullModel::~ConvexHullModel()
	{

	}

	Matrix4<float> ConvexHullModel::retrieveModelMatrix() const
	{
		return Matrix4<float>();
	}

	std::vector<Point3<float>> ConvexHullModel::retrieveVertexArray() const
	{
		std::vector<Point3<float>> vertexArray;
		vertexArray.reserve(convexHull.getIndexedTriangles().size()*3);

		auto &indexedPoints = convexHull.getIndexedPoints();
		for(auto &it : convexHull.getIndexedTriangles())
		{
			IndexedTriangle3D<float> triangle = it.second;

			Point3<float> point1 = indexedPoints.find(triangle.getIndexes()[0])->second;
			Point3<float> point2 = indexedPoints.find(triangle.getIndexes()[1])->second;
			Point3<float> point3 = indexedPoints.find(triangle.getIndexes()[2])->second;

			vertexArray.push_back(point1);
			vertexArray.push_back(point2);
			vertexArray.push_back(point3);
		}

		return vertexArray;
	}

	void ConvexHullModel::drawGeometry() const
	{
		glDrawArrays(GL_TRIANGLES, 0, convexHull.getIndexedTriangles().size()*3);
	}

}
