#include "CSGPolygon.h"

namespace urchin
{

	CSGPolygon::CSGPolygon(const std::vector<Point2<float>> &cwPoints) :
		cwPoints(cwPoints)
	{

	}

	const std::vector<Point2<float>> &CSGPolygon::getCwPoints() const
	{
		return cwPoints;
	}

}
