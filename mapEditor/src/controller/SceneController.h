#ifndef URCHINENGINE_SCENECONTROLLER_H
#define URCHINENGINE_SCENECONTROLLER_H

#include <list>

#include "UrchinMapHandler.h"
#include "controller/objects/ObjectController.h"
#include "controller/lights/LightController.h"
#include "controller/terrains/TerrainController.h"
#include "controller/waters/WaterController.h"
#include "controller/sky/SkyController.h"
#include "controller/sounds/SoundController.h"
#include "controller/ai/AIController.h"

namespace urchin {

    class SceneController : public AbstractController {
        public:
            SceneController();
            ~SceneController() override;

            void setup(MapHandler *) override;
            void addObserverOnAllControllers(Observer *, int);

            void forceModified();
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
