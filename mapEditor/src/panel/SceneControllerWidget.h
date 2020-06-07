#ifndef URCHINENGINE_MAPEDITOR_SCENECONTROLLERWIDGET_H
#define URCHINENGINE_MAPEDITOR_SCENECONTROLLERWIDGET_H

#include <string>
#include <QtWidgets/QTabWidget>

#include "UrchinCommon.h"
#include "UrchinMapHandler.h"
#include "controller/SceneController.h"
#include "panel/objects/ObjectControllerWidget.h"
#include "panel/lights/LightControllerWidget.h"
#include "panel/terrains/TerrainControllerWidget.h"
#include "panel/waters/WaterControllerWidget.h"
#include "panel/sky/SkyControllerWidget.h"
#include "panel/sounds/SoundControllerWidget.h"
#include "panel/ai/AIControllerWidget.h"

namespace urchin
{

	class SceneControllerWidget : public QTabWidget, public Observable
	{
		Q_OBJECT

		public:
			explicit SceneControllerWidget(QWidget *parent);

			enum NotificationType
			{
				TAB_SELECTED
			};

			enum TabName
			{
				OBJECTS,
				LIGHTS,
				TERRAINS,
				WATERS,
				SKY,
				SOUNDS,
				AI
			};

			ObjectControllerWidget *getObjectControllerWidget() const;
			LightControllerWidget *getLightControllerWidget() const;
			TerrainControllerWidget *getTerrainControllerWidget() const;
			WaterControllerWidget *getWaterControllerWidget() const;
            SkyControllerWidget *getSkyControllerWidget() const;
			SoundControllerWidget *getSoundControllerWidget() const;
			AIControllerWidget *getAIControllerWidget() const;

            SceneController *newMap(SceneController *);
            SceneController *openMap(SceneController *);
			void closeMap();

			TabName getTabSelected() const;

		private:
			SceneController *sceneController;

			ObjectControllerWidget *tabObjects;
			LightControllerWidget *tabLights;
			TerrainControllerWidget *tabTerrains;
			WaterControllerWidget *tabWaters;
			SkyControllerWidget *tabSky;
			SoundControllerWidget *tabSounds;
			AIControllerWidget *tabAI;

		private slots:
			void tabSelected();
	};

}

#endif
