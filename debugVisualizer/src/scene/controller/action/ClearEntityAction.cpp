#include "ClearEntityAction.h"

namespace urchin
{

	ClearEntityAction::ClearEntityAction(const std::string &entityId) :
		entityId(entityId),
		removedEntity(nullptr)
	{

	}

	ClearEntityAction::~ClearEntityAction()
	{

	}

	void ClearEntityAction::execute(std::shared_ptr<GeometryEntityHandler> geometryEntityHandler)
	{
		removedEntity = geometryEntityHandler->remove(entityId);
	}

	void ClearEntityAction::reverse(std::shared_ptr<GeometryEntityHandler> geometryEntityHandler)
	{
		geometryEntityHandler->add(removedEntity);
	}

}
