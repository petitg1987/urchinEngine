#include <stdexcept>

#include "Triangulation.h"
#include "path/navmesh/triangulation/MonotonePolygon.h"

namespace urchin
{

	TypedPointCmp::TypedPointCmp(const std::vector<Point2<float>> &ccwPolygonPoints) :
		ccwPolygonPoints(ccwPolygonPoints)
	{

	}

	Triangulation::Triangulation(const std::vector<Point2<float>> &ccwPolygonPoints) :
				ccwPolygonPoints(ccwPolygonPoints)
	{

	}


	void Triangulation::triangulate() const
	{ //based on "Computational Geometry - Algorithms and Applications, 3rd Ed" - "Polygon Triangulation"
		MonotonePolygon monotonePolygon(ccwPolygonPoints);
		monotonePolygon.createYMonotonePolygons();
	}
}
