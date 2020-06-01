#ifndef URCHINENGINE_MAPEDITOR_SCENECONTROLLER_H
#define URCHINENGINE_MAPEDITOR_SCENECONTROLLER_H

#include <list>

#include "UrchinMapHandler.h"
#include "panel/objects/ObjectController.h"
#include "panel/lights/LightController.h"
#include "panel/terrains/TerrainController.h"
#include "panel/waters/WaterController.h"
#include "panel/sky/SkyController.h"
#include "panel/sounds/SoundController.h"
#include "panel/ai/AIController.h"

namespace urchin
{

	class SceneController : public AbstractController
	{
		public:
			explicit SceneController(MapHandler *);
			~SceneController() override;

			void setRelativeWorkingDirectory(const std::string &);
			void addObserverOnAllControllers(Observer *, int);

            bool isModified() const override;
            void resetModified() override;

			void saveMapOnFile(const std::string &);

			ObjectController *getObjectController();
			LightController *getLightController();
			TerrainController *getTerrainController();
			WaterController *getWaterController();
            SkyController *getSkyController();
			SoundController *getSoundController();
			AIController *getAIController();

		private:
	        std::vector<AbstractController *> subControllers;

			ObjectController *objectController;
			LightController *lightController;
			TerrainController *terrainController;
            WaterController *waterController;
            SkyController *skyController;
			SoundController *soundController;
			AIController *aiController;
	};

}

#endif
