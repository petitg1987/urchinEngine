#include "CSGPolygon.h"

namespace urchin
{

	CSGPolygon::CSGPolygon(std::string name, const std::vector<Point2<float>> &cwPoints) :
		name(std::move(name)),
		cwPoints(cwPoints)
	{

	}

	CSGPolygon::CSGPolygon(const CSGPolygon &polygon) :
		name(polygon.name),
		cwPoints(polygon.cwPoints)
	{

	}

	CSGPolygon::CSGPolygon(CSGPolygon &&polygon) :
		name(std::move(polygon.name)),
		cwPoints(std::move(polygon.cwPoints))
	{

	}

	CSGPolygon& CSGPolygon::operator=(CSGPolygon &&polygon)
	{
		this->name = std::move(polygon.name);
		this->cwPoints = std::move(polygon.cwPoints);
		return *this;
	}

	const std::string &CSGPolygon::getName() const
	{
		return name;
	}

	const std::vector<Point2<float>> &CSGPolygon::getCwPoints() const
	{
		return cwPoints;
	}

	float CSGPolygon::computeArea() const
	{
		unsigned int n = cwPoints.size();
		cwPoints.reserve(n+2);
		cwPoints.push_back(cwPoints[0]);
		cwPoints.push_back(cwPoints[1]);

		float area = 0.0;
	    for (unsigned int i=1; i<=n; i++)
	    {
	        area -= cwPoints[i].X * (cwPoints[i+1].Y - cwPoints[i-1].Y);
	    }

		cwPoints.pop_back();
	    cwPoints.pop_back();

	    return area / 2.0f;
	}

	CSGPolygon CSGPolygon::expand(float distance) const
	{
		return CSGPolygon(name, ResizePolygon2DService<float>::instance()->resizePolygon(cwPoints, -distance));
	}

	std::ostream& operator <<(std::ostream &stream, const CSGPolygon &polygon)
	{
		stream << "Name:" << polygon.getName() << std::endl;
		stream << "Points (CW):" << std::endl;
		for(auto point : polygon.getCwPoints())
		{
			stream << point << std::endl;
		}
		return stream;
	}

}
