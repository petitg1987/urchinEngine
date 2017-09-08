#include "NavPolygon.h"

namespace urchin
{

	NavPolygon::NavPolygon(const std::vector<Point3<float>> &points, const std::vector<NavTriangle> &triangles) :
		points(points),
		triangles(triangles)
	{
		for(NavTriangle &triangle : this->triangles)
		{
			triangle.computeCenterPoint(points);
		}
	}

	const std::vector<Point3<float>> &NavPolygon::getPoints() const
	{
		return points;
	}

	const Point3<float> &NavPolygon::getPoint(unsigned int index) const
	{
		return points[index];
	}

	const std::vector<NavTriangle> &NavPolygon::getTriangles() const
	{
		return triangles;
	}

	const NavTriangle &NavPolygon::getTriangle(unsigned int index) const
	{
		return triangles[index];
	}

}
