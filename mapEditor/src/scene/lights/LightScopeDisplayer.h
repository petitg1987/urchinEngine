#pragma once

#include <Urchin3dEngine.h>
#include <UrchinAggregation.h>

namespace urchin {

    class LightScopeDisplayer { //TODO move inside objects folder
        public:
            explicit LightScopeDisplayer(Scene&);
            ~LightScopeDisplayer();

            void displayLightScope(const ObjectEntity*);

        private:
            void cleanCurrentDisplay();

            Scene& scene;
            std::vector<std::shared_ptr<GeometryModel>> lightScopeModels;
    };

}
