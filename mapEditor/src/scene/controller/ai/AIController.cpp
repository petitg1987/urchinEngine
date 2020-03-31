#include <memory>
#include "UrchinAIEngine.h"

#include "AIController.h"

namespace urchin
{

	AIController::AIController(MapHandler *mapHandler) :
			AbstractController(mapHandler)
	{

	}

	const SceneAI *AIController::getSceneAI() const
	{
		return getMapHandler()->getMap()->getSceneAI();
	}

	const SceneAI *AIController::updateSceneAI(const std::shared_ptr<NavMeshAgent> &navMeshAgent)
	{
		getMapHandler()->getMap()->updateSceneAI(navMeshAgent);

		markModified();
		return getMapHandler()->getMap()->getSceneAI();
	}

}
