#include <memory>
#include <utility>
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

	const SceneAI * AIController::getSceneAI() const
	{
		return mapHandler->getMap()->getSceneAI();
	}

	SceneAI *AIController::updateNavMeshConfig(std::shared_ptr<NavMeshConfig> navMeshConfig)
	{
		SceneAI *sceneAI = mapHandler->getMap()->getSceneAI();

		sceneAI->changeNavMeshConfig(std::move(navMeshConfig));

		markModified();
		return sceneAI;
	}

}
