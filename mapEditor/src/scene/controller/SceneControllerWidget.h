#ifndef ENGINE_MAPEDITOR_SCENECONTROLLERWIDGET_H
#define ENGINE_MAPEDITOR_SCENECONTROLLERWIDGET_H

#include <string>
#include <QtWidgets/QTabWidget>

#include "UrchinCommon.h"
#include "UrchinMapHandler.h"
#include "scene/controller/SceneController.h"
#include "scene/controller/objects/ObjectControllerWidget.h"
#include "scene/controller/lights/LightControllerWidget.h"
#include "scene/controller/sounds/SoundControllerWidget.h"

namespace urchin
{

	class SceneControllerWidget : public QTabWidget
	{
		Q_OBJECT

		public:
			SceneControllerWidget(QWidget *parent);
			virtual ~SceneControllerWidget();

			ObjectControllerWidget *getObjectControllerWidget() const;
			LightControllerWidget *getLightControllerWidget() const;
			SoundControllerWidget *getSoundControllerWidget() const;

			bool isModified() const;
			bool hasMapOpen() const;

			void newMap(MapHandler *, const std::string &);
			void openMap(MapHandler *);
			void saveMap(const std::string &);
			void closeMap();

		private:
			SceneController *sceneController;

			ObjectControllerWidget *tabObjects;
			LightControllerWidget *tabLights;
			SoundControllerWidget *tabSounds;
	};

}

#endif
