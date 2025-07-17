#pragma once

#include <Urchin3dEngine.h>
#include <UrchinAggregation.h>

namespace urchin {

    class LightScopeDisplayer {
        public:
            explicit LightScopeDisplayer(Scene&);
            ~LightScopeDisplayer();

            void displayLightScopes(const std::vector<const ObjectEntity*>&);

        private:
            void cleanCurrentDisplay();

            Scene& scene;
            std::vector<std::shared_ptr<GeometryModel>> lightScopeModels;
    };

}
