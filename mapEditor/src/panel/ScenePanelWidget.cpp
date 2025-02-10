#include <stdexcept>

#include <panel/ScenePanelWidget.h>

namespace urchin {

    ScenePanelWidget::ScenePanelWidget(QWidget* parent) :
            QTabWidget(parent),
            sceneController(nullptr) {
        tabObjects = new ObjectPanelWidget();
        addTab(tabObjects, "Object");

        tabTerrains = new TerrainPanelWidget();
        addTab(tabTerrains, "Terrain");

        tabWaters = new WaterPanelWidget();
        addTab(tabWaters, "Water");

        tabSky = new SkyPanelWidget();
        addTab(tabSky, "Sky");

        tabAI = new AIPanelWidget();
        addTab(tabAI, "AI");

        connect(this, SIGNAL(currentChanged(int)), this, SLOT(tabSelected()));
        setCurrentIndex(0);
        setEnabled(false);
    }

    ObjectPanelWidget* ScenePanelWidget::getObjectPanelWidget() const {
        return tabObjects;
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

    AIPanelWidget* ScenePanelWidget::getAIPanelWidget() const {
        return tabAI;
    }

    SceneController& ScenePanelWidget::loadMap(SceneController& sceneController) {
        closeMap();
        this->sceneController = &sceneController;

        setEnabled(true);
        tabObjects->load(sceneController.getObjectController());
        tabTerrains->load(sceneController.getTerrainController());
        tabWaters->load(sceneController.getWaterController());
        tabSky->load(sceneController.getSkyController());
        tabAI->load(sceneController.getAIController());

        return sceneController;
    }

    void ScenePanelWidget::closeMap() {
        tabObjects->unload();
        tabTerrains->unload();
        tabWaters->unload();
        tabSky->unload();
        tabAI->unload();
        setEnabled(false);

        sceneController = nullptr;
    }

    ScenePanelWidget::MainTab ScenePanelWidget::getTabSelected() const {
        int tabIndex = this->currentIndex();
        if (tabIndex == 0) {
            return MainTab::OBJECTS;
        } else if (tabIndex == 1) {
            return MainTab::TERRAINS;
        } else if (tabIndex == 2) {
            return MainTab::WATERS;
        } else if (tabIndex == 3) {
            return MainTab::SKY;
        } else if (tabIndex == 4) {
            return MainTab::AI;
        }

        throw std::runtime_error("Unknown selected tab index: " + std::to_string(tabIndex));
    }

    void ScenePanelWidget::tabSelected() {
        notifyObservers(this, TAB_SELECTED);
    }
}
