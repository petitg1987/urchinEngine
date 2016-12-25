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

	AIController::~AIController()
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

	void AIController::generateNavMesh(float agentHeight, float agentRadius)
	{
		NavMeshConfig navMeshConfig(agentHeight, agentRadius);
		std::shared_ptr<AIWorld> aiWorld = mapHandler->generateAIWorld();

		NavMeshGenerator navMeshGenerator(aiWorld, navMeshConfig);
		std::shared_ptr<NavMesh> navMesh = navMeshGenerator.generate();

		markModified();
	}

}
