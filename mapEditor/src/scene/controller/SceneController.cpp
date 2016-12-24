#include <stdexcept>

#include "SceneController.h"

namespace urchin
{

	SceneController::SceneController(MapHandler *mapHandler) :
			bIsModified(false),
			mapHandler(mapHandler)
	{
		objectController = new ObjectController(mapHandler);
		lightController = new LightController(mapHandler);
		soundController = new SoundController(mapHandler);
		aiController = new AIController(mapHandler);
	}

	SceneController::~SceneController()
	{
		delete objectController;
		delete lightController;
		delete soundController;
		delete aiController;
	}

	void SceneController::setRelativeWorkingDirectory(const std::string &relativeWorkingDirectory)
	{
		mapHandler->setRelativeWorkingDirectory(relativeWorkingDirectory);

		markModified();
	}

	bool SceneController::isModified() const
	{
		return bIsModified
				|| objectController->isModified()
				|| lightController->isModified()
				|| soundController->isModified()
				|| aiController->isModified();
	}

	void SceneController::markModified()
	{
		bIsModified = true;
	}

	void SceneController::resetModified()
	{
		bIsModified = false;
		objectController->resetModified();
		lightController->resetModified();
		soundController->resetModified();
		aiController->resetModified();
	}

	void SceneController::saveMapOnFile(const std::string &mapFilename)
	{
		std::string tmpResourcesDirectory = FileSystem::instance()->getResourcesDirectory();
		FileSystem::instance()->setupResourcesDirectory("");
		mapHandler->writeMapOnFile(mapFilename);
		FileSystem::instance()->setupResourcesDirectory(tmpResourcesDirectory);

		resetModified();
	}

	ObjectController *SceneController::getObjectController()
	{
		return objectController;
	}

	LightController *SceneController::getLightController()
	{
		return lightController;
	}

	SoundController *SceneController::getSoundController()
	{
		return soundController;
	}

	AIController *SceneController::getAIController()
	{
		return aiController;
	}

}
