#include "NavPolygon.h"

namespace urchin
{

	NavPolygon::NavPolygon(std::string name, const std::vector<Point3<float>> &points, const std::vector<NavTriangle> &triangles) :
        	name(std::move(name)),
			points(points),
			triangles(triangles)
	{
		for(NavTriangle &triangle : this->triangles)
		{
			triangle.computeCenterPoint(points);
		}
	}

	NavPolygon::NavPolygon(const NavPolygon &navPolygon) :
			name(navPolygon.getName()),
			points(navPolygon.getPoints()),
			triangles(navPolygon.getTriangles())
	{

	}

    const std::string &NavPolygon::getName() const
    {
        return name;
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
