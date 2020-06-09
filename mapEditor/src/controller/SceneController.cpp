#include <stdexcept>

#include "SceneController.h"

namespace urchin
{

	SceneController::SceneController(MapHandler *mapHandler) :
            AbstractController(mapHandler)
	{
		objectController = new ObjectController(mapHandler);
        subControllers.emplace_back(objectController);

		lightController = new LightController(mapHandler);
        subControllers.emplace_back(lightController);

		terrainController = new TerrainController(mapHandler);
        subControllers.emplace_back(terrainController);

		waterController = new WaterController(mapHandler);
        subControllers.emplace_back(waterController);

		skyController = new SkyController(mapHandler);
        subControllers.emplace_back(skyController);

		soundController = new SoundController(mapHandler);
        subControllers.emplace_back(soundController);

		aiController = new AIController(mapHandler);
        subControllers.emplace_back(aiController);
	}

	SceneController::~SceneController()
	{
	    for(const auto &subController: subControllers)
        {
	        delete subController;
        }
	}

    void SceneController::addObserverOnAllControllers(Observer *observer, int notificationType)
    {
	    this->addObserver(observer, notificationType);
        for(const auto &subController: subControllers)
        {
            subController->addObserver(observer, notificationType);
        }
    }

    void SceneController::forceModified()
    {
         AbstractController::markModified();
    }

	bool SceneController::isModified() const
	{
	    if(AbstractController::isModified())
        {
	        return true;
        }

        for(const auto &subController: subControllers)
        {
            if(subController->isModified())
            {
                return true;
            }
        }

        return false;
	}

	void SceneController::resetModified()
	{
        AbstractController::resetModified();
        for(const auto &subController: subControllers)
        {
            subController->resetModified();
        }
	}

	void SceneController::saveMapOnFile(const std::string &mapFilename)
	{
		std::string tmpResourcesDirectory = FileSystem::instance()->getResourcesDirectory();
		FileSystem::instance()->setupResourcesDirectory("");
		getMapHandler()->writeMapOnFile(mapFilename);
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

	TerrainController *SceneController::getTerrainController()
	{
		return terrainController;
	}

	WaterController *SceneController::getWaterController()
	{
		return waterController;
	}

    SkyController *SceneController::getSkyController()
    {
        return skyController;
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
