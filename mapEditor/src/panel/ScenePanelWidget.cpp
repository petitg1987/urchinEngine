#include <stdexcept>

#include <panel/ScenePanelWidget.h>

namespace urchin {

    ScenePanelWidget::ScenePanelWidget(QWidget* parent) :
            QTabWidget(parent),
            sceneController(nullptr) {
        tabObjects = new ModelPanelWidget();
        addTab(tabObjects, "Object");

        tabLights = new LightPanelWidget();
        addTab(tabLights, "Light");

        tabTerrains = new TerrainPanelWidget();
        addTab(tabTerrains, "Terrain");

        tabWaters = new WaterPanelWidget();
        addTab(tabWaters, "Water");

        tabSky = new SkyPanelWidget();
        addTab(tabSky, "Sky");

        tabSounds = new SoundPanelWidget();
        addTab(tabSounds, "Sound");

        tabAI = new AIPanelWidget();
        addTab(tabAI, "AI");

        connect(this, SIGNAL(currentChanged(int)), this, SLOT(tabSelected()));
        setCurrentIndex(0);
        setEnabled(false);
    }

    ModelPanelWidget* ScenePanelWidget::getObjectPanelWidget() const {
        return tabObjects;
    }

    LightPanelWidget* ScenePanelWidget::getLightPanelWidget() const {
        return tabLights;
    }

    TerrainPanelWidget* ScenePanelWidget::getTerrainPanelWidget() const {
        return tabTerrains;
    }

    WaterPanelWidget* ScenePanelWidget::getWaterPanelWidget() const {
        return tabWaters;
    }

    SkyPanelWidget* ScenePanelWidget::getSkyPanelWidget() const {
        return tabSky;
    }

    SoundPanelWidget* ScenePanelWidget::getSoundPanelWidget() const {
        return tabSounds;
    }

    AIPanelWidget* ScenePanelWidget::getAIPanelWidget() const {
        return tabAI;
    }

    SceneController& ScenePanelWidget::loadMap(SceneController& sceneController) {
        closeMap();
        this->sceneController = &sceneController;

        setEnabled(true);
        tabObjects->load(sceneController.getObjectController());
        tabLights->load(sceneController.getLightController());
        tabTerrains->load(sceneController.getTerrainController());
        tabWaters->load(sceneController.getWaterController());
        tabSky->load(sceneController.getSkyController());
        tabSounds->load(sceneController.getSoundController());
        tabAI->load(sceneController.getAIController());

        return sceneController;
    }

    void ScenePanelWidget::closeMap() {
        tabObjects->unload();
        tabLights->unload();
        tabTerrains->unload();
        tabWaters->unload();
        tabSky->unload();
        tabSounds->unload();
        tabAI->unload();
        setEnabled(false);

        sceneController = nullptr;
    }

    ScenePanelWidget::TabName ScenePanelWidget::getTabSelected() const {
        int tabIndex = this->currentIndex();
        if (tabIndex == 0) {
            return TabName::OBJECTS;
        } else if (tabIndex == 1) {
            return TabName::LIGHTS;
        } else if (tabIndex == 2) {
            return TabName::TERRAINS;
        } else if (tabIndex == 3) {
            return TabName::WATERS;
        } else if (tabIndex == 4) {
            return TabName::SKY;
        } else if (tabIndex == 5) {
            return TabName::SOUNDS;
        } else if (tabIndex == 6) {
            return TabName::AI;
        }

        throw std::runtime_error("Unknown selected tab index: " + std::to_string(tabIndex));
    }

    void ScenePanelWidget::tabSelected() {
        notifyObservers(this, NotificationType::TAB_SELECTED);
    }
}
