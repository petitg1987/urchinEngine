#pragma once

#include <list>

#include <UrchinMapHandler.h>
#include <controller/objects/ObjectController.h>
#include <controller/lights/LightController.h>
#include <controller/terrains/TerrainController.h>
#include <controller/waters/WaterController.h>
#include <controller/sky/SkyController.h>
#include <controller/sounds/SoundController.h>
#include <controller/ai/AIController.h>

namespace urchin {

    class SceneController : public AbstractController {
        public:
            SceneController();

            void setup(MapHandler*) override;
            void addObserverOnAllControllers(Observer*, int);

            void forceModified();
            bool isModified() const override;
            void resetModified() override;

            void saveMapOnFile(const std::string&);

            ObjectController& getModelController();
            LightController& getLightController();
            TerrainController& getTerrainController();
            WaterController& getWaterController();
            SkyController& getSkyController();
            SoundController& getSoundController();
            AIController& getAIController();

        private:
            std::vector<AbstractController*> subControllers;

            std::unique_ptr<ObjectController> modelController;
            std::unique_ptr<LightController> lightController;
            std::unique_ptr<TerrainController> terrainController;
            std::unique_ptr<WaterController> waterController;
            std::unique_ptr<SkyController> skyController;
            std::unique_ptr<SoundController> soundController;
            std::unique_ptr<AIController> aiController;
    };

}
