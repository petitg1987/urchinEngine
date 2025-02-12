#pragma once

#include <string>

#include <UrchinCommon.h>
#include <controller/SceneController.h>
#include <panel/object/ObjectPanelWidget.h>
#include <panel/terrain/TerrainPanelWidget.h>
#include <panel/water/WaterPanelWidget.h>
#include <panel/sky/SkyPanelWidget.h>
#include <panel/ai/AIPanelWidget.h>

namespace urchin {

    class ScenePanelWidget final : public QTabWidget, public Observable {
        Q_OBJECT

        public:
            explicit ScenePanelWidget(QWidget*);

            enum NotificationType {
                TAB_SELECTED
            };

            enum class MainTab {
                OBJECTS,
                TERRAINS,
                WATERS,
                SKY,
                AI
            };

            ObjectPanelWidget* getObjectPanelWidget() const;
            TerrainPanelWidget* getTerrainPanelWidget() const;
            WaterPanelWidget* getWaterPanelWidget() const;
            SkyPanelWidget* getSkyPanelWidget() const;
            AIPanelWidget* getAIPanelWidget() const;

            SceneController& loadMap(SceneController&);
            void closeMap();

            MainTab getTabSelected() const;

        private:
            SceneController* sceneController;

            ObjectPanelWidget* tabObjects;
            TerrainPanelWidget* tabTerrains;
            WaterPanelWidget* tabWaters;
            SkyPanelWidget* tabSky;
            AIPanelWidget* tabAI;

        private slots:
            void tabSelected();
    };

}
