#pragma once

#include <Urchin3dEngine.h>
#include <UrchinMapHandler.h>

namespace urchin {

    class LightScopeDisplayer {
        public:
            explicit LightScopeDisplayer(Scene&);
            ~LightScopeDisplayer();

            void displayLightScope(const SceneLight*);

        private:
            void cleanCurrentDisplay();

            Scene& scene;
            std::vector<std::shared_ptr<GeometryModel>> lightScopeModels;
    };

}
