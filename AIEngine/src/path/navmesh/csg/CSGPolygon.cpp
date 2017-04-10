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

	const std::string &CSGPolygon::getName() const
	{
		return name;
	}

	CSGPolygon& CSGPolygon::operator=(CSGPolygon &&polygon)
	{
		this->name = std::move(polygon.name);
		this->cwPoints = std::move(polygon.cwPoints);
		return *this;
	}

	const std::vector<Point2<float>> &CSGPolygon::getCwPoints() const
	{
		return cwPoints;
	}

	std::ostream& operator <<(std::ostream &stream, const CSGPolygon &polygon)
	{
		stream << polygon.getName() << std::endl;
		for(auto point : polygon.getCwPoints())
		{
			stream << "- " << point << std::endl;
		}
		stream<<std::endl;
		return stream;
	}

}
