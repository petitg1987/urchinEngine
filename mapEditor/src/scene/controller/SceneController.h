#ifndef URCHINENGINE_MAPEDITOR_SCENECONTROLLER_H
#define URCHINENGINE_MAPEDITOR_SCENECONTROLLER_H

#include <list>

#include "UrchinMapHandler.h"
#include "scene/controller/objects/ObjectController.h"
#include "scene/controller/lights/LightController.h"
#include "scene/controller/sounds/SoundController.h"
#include "scene/controller/ai/AIController.h"

namespace urchin
{

	class SceneController
	{
		public:
			SceneController(MapHandler *);
			~SceneController();

			void setRelativeWorkingDirectory(const std::string &);
			bool isModified() const;

			void saveMapOnFile(const std::string &);

			ObjectController *getObjectController();
			LightController *getLightController();
			SoundController *getSoundController();
			AIController *getAIController();

		private:
			void markModified();
			void resetModified();

			bool bIsModified;
			MapHandler *mapHandler;

			ObjectController *objectController;
			LightController *lightController;
			SoundController *soundController;
			AIController *aiController;
	};

}

#endif
