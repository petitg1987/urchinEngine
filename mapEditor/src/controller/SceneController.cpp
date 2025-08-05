#include "controller/SceneController.h"

namespace urchin {

    SceneController::SceneController() {
        objectController = std::make_unique<ObjectController>();
        subControllers.emplace_back(objectController.get());

        terrainController = std::make_unique<TerrainController>();
        subControllers.emplace_back(terrainController.get());

        waterController = std::make_unique<WaterController>();
        subControllers.emplace_back(waterController.get());

        skyController = std::make_unique<SkyController>();
        subControllers.emplace_back(skyController.get());

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
         markModified();
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

    const std::array<std::string, 8>& SceneController::getLightMaskNames() const {
        return getMap().getLightMaskNames();
    }

    void SceneController::setLightMaskNames(std::array<std::string, 8> lightMaskName) {
        getMap().setLightMaskNames(std::move(lightMaskName));
        markModified();
    }

    ObjectController& SceneController::getObjectController() const {
        return *objectController;
    }

    TerrainController& SceneController::getTerrainController() const {
        return *terrainController;
    }

    WaterController& SceneController::getWaterController() const {
        return *waterController;
    }

    SkyController& SceneController::getSkyController() const {
        return *skyController;
    }

    AIController& SceneController::getAIController() const {
        return *aiController;
    }

}
