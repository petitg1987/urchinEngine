#include "CSGPolygon.h"

namespace urchin
{

	CSGPolygon::CSGPolygon(const std::vector<Point2<float>> &cwPoints) :
		cwPoints(cwPoints)
	{

	}

	CSGPolygon::CSGPolygon(const CSGPolygon &polygon) :
		cwPoints(polygon.cwPoints)
	{

	}

	CSGPolygon::CSGPolygon(CSGPolygon &&polygon) :
		cwPoints(std::move(polygon.cwPoints))
	{

	}

	CSGPolygon& CSGPolygon::operator=(CSGPolygon &&polygon)
	{
		this->cwPoints = std::move(polygon.cwPoints);
		return *this;
	}

	const std::vector<Point2<float>> &CSGPolygon::getCwPoints() const
	{
		return cwPoints;
	}

}
