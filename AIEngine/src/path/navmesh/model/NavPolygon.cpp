#include "NavPolygon.h"

namespace urchin
{

	NavPolygon::NavPolygon()
	{

	}

	NavPolygon::NavPolygon(const std::vector<Point3<float>> &points) :
		points(points)
	{

	}

	unsigned int NavPolygon::addPoint(const Point3<float> &point)
	{
		points.push_back(point);
		return points.size()-1;
	}

	const std::vector<Point3<float>> &NavPolygon::getPoints() const
	{
		return points;
	}

	const Point3<float> &NavPolygon::getPoint(unsigned int index) const
	{
		return points[index];
	}
}
