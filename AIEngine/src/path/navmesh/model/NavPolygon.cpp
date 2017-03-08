#include "NavPolygon.h"

namespace urchin
{

	NavPolygon::NavPolygon(const std::vector<Point3<float>> &points, const std::vector<IndexedTriangle2D<float>> &triangles) : //TODO should be IndexedTriangle3D ???
		points(points),
		triangles(triangles)
	{

	}

	const std::vector<Point3<float>> &NavPolygon::getPoints() const
	{
		return points;
	}

	const Point3<float> &NavPolygon::getPoint(unsigned int index) const
	{
		return points[index];
	}

	const std::vector<IndexedTriangle2D<float>> &NavPolygon::getTriangles() const
	{
		return triangles;
	}

	const IndexedTriangle2D<float> &NavPolygon::getTriangle(unsigned int index) const
	{
		return triangles[index];
	}
}
