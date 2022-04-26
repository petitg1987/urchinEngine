#include <stdexcept>

#include <controller/SceneController.h>

namespace urchin {

    SceneController::SceneController() :
            AbstractController() {
        objectController = std::make_unique<ObjectController>();
        subControllers.emplace_back(objectController.get());

        lightController = std::make_unique<LightController>();
        subControllers.emplace_back(lightController.get());

        terrainController = std::make_unique<TerrainController>();
        subControllers.emplace_back(terrainController.get());

        waterController = std::make_unique<WaterController>();
        subControllers.emplace_back(waterController.get());

        skyController = std::make_unique<SkyController>();
        subControllers.emplace_back(skyController.get());

        soundController = std::make_unique<SoundController>();
        subControllers.emplace_back(soundController.get());

        aiController = std::make_unique<AIController>();
        subControllers.emplace_back(aiController.get());
    }

    void SceneController::setup(Map& map) {
        AbstractController::setup(map);
        for (const auto& subController: subControllers) {
            subController->setup(map);
        }
    }

    void SceneController::addObserverOnAllControllers(Observer* observer, int notificationType) {
        this->addObserver(observer, notificationType);
        for (const auto& subController: subControllers) {
            subController->addObserver(observer, notificationType);
        }
    }

    void SceneController::forceModified() {
         AbstractController::markModified();
    }

    bool SceneController::isModified() const {
        if (AbstractController::isModified()) {
            return true;
        }

        return std::ranges::any_of(subControllers, [](const auto& subController) {
            return subController->isModified();
        });
    }

    void SceneController::resetModified() {
        AbstractController::resetModified();
        for (const auto& subController: subControllers) {
            subController->resetModified();
        }
    }

    void SceneController::saveMapOnFile(const std::string& mapFilename) {
        MapSaveService().saveMap(mapFilename, getMap());
        resetModified();
    }

    ObjectController& SceneController::getObjectController() {
        return *objectController;
    }

    LightController& SceneController::getLightController() {
        return *lightController;
    }

    TerrainController& SceneController::getTerrainController() {
        return *terrainController;
    }

    WaterController& SceneController::getWaterController() {
        return *waterController;
    }

    SkyController& SceneController::getSkyController() {
        return *skyController;
    }

    SoundController& SceneController::getSoundController() {
        return *soundController;
    }

    AIController& SceneController::getAIController() {
        return *aiController;
    }

}
