#include <stdexcept>

#include "SceneController.h"

namespace urchin
{

    SceneController::SceneController() :
            AbstractController()
    {
        objectController = new ObjectController();
        subControllers.emplace_back(objectController);

        lightController = new LightController();
        subControllers.emplace_back(lightController);

        terrainController = new TerrainController();
        subControllers.emplace_back(terrainController);

        waterController = new WaterController();
        subControllers.emplace_back(waterController);

        skyController = new SkyController();
        subControllers.emplace_back(skyController);

        soundController = new SoundController();
        subControllers.emplace_back(soundController);

        aiController = new AIController();
        subControllers.emplace_back(aiController);
    }

    SceneController::~SceneController()
    {
        for(const auto &subController: subControllers)
        {
            delete subController;
        }
    }

    void SceneController::setup(MapHandler *mapHandler)
    {
        AbstractController::setup(mapHandler);

        objectController->setup(mapHandler);
        lightController->setup(mapHandler);
        terrainController->setup(mapHandler);
        waterController->setup(mapHandler);
        skyController->setup(mapHandler);
        soundController->setup(mapHandler);
        aiController->setup(mapHandler);
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
