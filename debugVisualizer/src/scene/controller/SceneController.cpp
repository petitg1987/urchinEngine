#include <stdexcept>

#include "SceneController.h"
#include "scene/controller/ActionReader.h"

namespace urchin
{

	SceneController::SceneController() :
		bHasDebugFileOpen(false)
	{

	}

	SceneController::~SceneController()
	{

	}

	void SceneController::openDebugFile(const std::string &filename, std::shared_ptr<GeometryEntityHandler> geometryEntityHandler)
	{
		this->geometryEntityHandler = geometryEntityHandler;
		this->bHasDebugFileOpen = true;

		ActionReader actionReader(filename);
		actions = actionReader.readDebugFile();

		actionsIt = actions.begin();
	}

	bool SceneController::hasDebugFileOpen()
	{
		return bHasDebugFileOpen;
	}

	void SceneController::executeNextAction()
	{
		if(actionsIt!=actions.end())
		{
			(*actionsIt)->execute(geometryEntityHandler);
			actionsIt++;
		}
	}

	void SceneController::reverseLastAction()
	{
		if(actionsIt!=actions.begin())
		{
			actionsIt--;
			(*actionsIt)->reverse(geometryEntityHandler);
		}
	}

}
