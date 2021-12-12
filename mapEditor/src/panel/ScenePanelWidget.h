#pragma once

#include <string>
#include <QtWidgets/QTabWidget>

#include <UrchinCommon.h>
#include <UrchinAggregation.h>
#include <controller/SceneController.h>
#include <panel/objects/ObjectPanelWidget.h>
#include <panel/lights/LightPanelWidget.h>
#include <panel/terrains/TerrainPanelWidget.h>
#include <panel/waters/WaterPanelWidget.h>
#include <panel/sky/SkyPanelWidget.h>
#include <panel/sounds/SoundPanelWidget.h>
#include <panel/ai/AIPanelWidget.h>

namespace urchin {

    class ScenePanelWidget : public QTabWidget, public Observable {
        Q_OBJECT

        public:
            explicit ScenePanelWidget(QWidget*);

            enum NotificationType {
                TAB_SELECTED
            };

            enum TabName {
                OBJECTS,
                LIGHTS,
                TERRAINS,
                WATERS,
                SKY,
                SOUNDS,
                AI
            };

            ObjectPanelWidget* getObjectPanelWidget() const;
            LightPanelWidget* getLightPanelWidget() const;
            TerrainPanelWidget* getTerrainPanelWidget() const;
            WaterPanelWidget* getWaterPanelWidget() const;
            SkyPanelWidget* getSkyPanelWidget() const;
            SoundPanelWidget* getSoundPanelWidget() const;
            AIPanelWidget* getAIPanelWidget() const;

            SceneController& loadMap(SceneController&);
            void closeMap();

            TabName getTabSelected() const;

        private:
            SceneController* sceneController;

            ObjectPanelWidget* tabObjects;
            LightPanelWidget* tabLights;
            TerrainPanelWidget* tabTerrains;
            WaterPanelWidget* tabWaters;
            SkyPanelWidget* tabSky;
            SoundPanelWidget* tabSounds;
            AIPanelWidget* tabAI;

        private slots:
            void tabSelected();
    };

}
