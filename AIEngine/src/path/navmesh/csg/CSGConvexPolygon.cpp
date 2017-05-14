#include "CSGConvexPolygon.h"

namespace urchin
{
	CSGConvexPolygon::CSGConvexPolygon(std::string name, const std::vector<Point2<float>> &cwPoints) :
		CSGPolygon(std::move(name), cwPoints)
	{

	}

	CSGConvexPolygon::CSGConvexPolygon(const CSGConvexPolygon &convexPolygon) :
		CSGPolygon(convexPolygon.getName(), convexPolygon.getCwPoints())
	{

	}

	CSGConvexPolygon::CSGConvexPolygon(CSGConvexPolygon &&convexPolygon) :
		CSGPolygon(std::move(convexPolygon.getName()), std::move(convexPolygon.getCwPoints()))
	{

	}
}
