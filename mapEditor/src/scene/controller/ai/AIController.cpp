#include <memory>
#include "UrchinAIEngine.h"

#include "AIController.h"

namespace urchin
{

	AIController::AIController(MapHandler *mapHandler) :
			bIsModified(false),
			mapHandler(mapHandler)
	{

	}

	bool AIController::isModified() const
	{
		return bIsModified;
	}

	void AIController::markModified()
	{
		bIsModified = true;
	}

	void AIController::resetModified()
	{
		bIsModified = false;
	}

	const SceneAI *AIController::getSceneAI() const
	{
		return mapHandler->getMap()->getSceneAI();
	}

	const SceneAI *AIController::updateSceneAI(const std::shared_ptr<NavMeshAgent> &navMeshAgent)
	{
		mapHandler->getMap()->updateSceneAI(navMeshAgent);

		markModified();
		return mapHandler->getMap()->getSceneAI();
	}

}
