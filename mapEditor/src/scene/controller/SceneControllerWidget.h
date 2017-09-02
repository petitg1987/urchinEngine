#ifndef URCHINENGINE_MAPEDITOR_SCENECONTROLLERWIDGET_H
#define URCHINENGINE_MAPEDITOR_SCENECONTROLLERWIDGET_H

#include <string>
#include <QtWidgets/QTabWidget>

#include "UrchinCommon.h"
#include "UrchinMapHandler.h"
#include "scene/controller/SceneController.h"
#include "scene/controller/objects/ObjectControllerWidget.h"
#include "scene/controller/lights/LightControllerWidget.h"
#include "scene/controller/sounds/SoundControllerWidget.h"
#include "scene/controller/ai/AIControllerWidget.h"

namespace urchin
{

	class SceneControllerWidget : public QTabWidget, public Observable
	{
		Q_OBJECT

		public:
			SceneControllerWidget(QWidget *parent);
			virtual ~SceneControllerWidget();

			enum NotificationType
			{
				TAB_SELECTED
			};

			enum TabName
			{
				OBJECTS,
				LIGHTS,
				SOUNDS,
				AI
			};

			ObjectControllerWidget *getObjectControllerWidget() const;
			LightControllerWidget *getLightControllerWidget() const;
			SoundControllerWidget *getSoundControllerWidget() const;
			AIControllerWidget *getAIControllerWidget() const;

			bool isModified() const;
			bool hasMapOpen() const;

			void newMap(MapHandler *, const std::string &);
			void openMap(MapHandler *);
			void saveMap(const std::string &);
			void closeMap();

			TabName getTabSelected() const;

		private:
			SceneController *sceneController;

			ObjectControllerWidget *tabObjects;
			LightControllerWidget *tabLights;
			SoundControllerWidget *tabSounds;
			AIControllerWidget *tabAI;

		private slots:
			void tabSelected();
	};

}

#endif
