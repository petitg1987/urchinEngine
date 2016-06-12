#include <vector>

#include "ConvexHull3DAction.h"

namespace urchin
{

	ConvexHull3DAction::ConvexHull3DAction(const std::string &entityId, const ConvexHull3D<float> &convexHull)
	{
		entity = new GeometryEntity(entityId, std::make_shared<ConvexHullModel>(convexHull));
	}

	ConvexHull3DAction::~ConvexHull3DAction()
	{
		delete entity;
	}

	void ConvexHull3DAction::execute(std::shared_ptr<GeometryEntityHandler> geometryEntityHandler)
	{
		geometryEntityHandler->add(entity);
	}

	void ConvexHull3DAction::reverse(std::shared_ptr<GeometryEntityHandler> geometryEntityHandler)
	{
		geometryEntityHandler->remove(entity);
	}

}
