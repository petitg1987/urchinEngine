#include <vector>

#include "Point3Action.h"

namespace urchin
{

	Point3Action::Point3Action(const std::string &entityId, const Point3<float> &point)
	{
		std::vector<Point3<float>> points(1);
		points.push_back(point);

		entity = new GeometryEntity(entityId, std::make_shared<PointsModel>(points, 4));
	}

	Point3Action::~Point3Action()
	{
		delete entity;
	}

	void Point3Action::execute(std::shared_ptr<GeometryEntityHandler> geometryEntityHandler)
	{
		geometryEntityHandler->add(entity);
	}

	void Point3Action::reverse(std::shared_ptr<GeometryEntityHandler> geometryEntityHandler)
	{
		geometryEntityHandler->remove(entity);
	}

}
