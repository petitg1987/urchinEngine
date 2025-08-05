#pragma once

#include <vector>
#include <memory>
#include <UrchinAggregation.h>

#include "controller/object/ObjectController.h"
#include "controller/terrain/TerrainController.h"
#include "controller/water/WaterController.h"
#include "controller/sky/SkyController.h"
#include "controller/ai/AIController.h"

namespace urchin {

    class SceneController final : public AbstractController {
        public:
            SceneController();

            enum SceneNotificationType {
                LIGHT_MASK_NAMES_UPDATED = 100
            };

            void setup(Map&) override;
            void addObserverOnAllControllers(Observer*, int);

            void forceModified();
            bool isModified() const override;
            void resetModified() override;

            void saveMapOnFile(const std::string&);

            const std::array<std::string, 8>& getLightMaskNames() const;
            void setLightMaskNames(std::array<std::string, 8>);

            ObjectController& getObjectController() const;
            TerrainController& getTerrainController() const;
            WaterController& getWaterController() const;
            SkyController& getSkyController() const;
            AIController& getAIController() const;

        private:
            std::vector<AbstractController*> subControllers;

            std::unique_ptr<ObjectController> objectController;
            std::unique_ptr<TerrainController> terrainController;
            std::unique_ptr<WaterController> waterController;
            std::unique_ptr<SkyController> skyController;
            std::unique_ptr<AIController> aiController;
    };

}
