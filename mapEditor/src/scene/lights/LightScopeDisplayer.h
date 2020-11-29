#ifndef URCHINENGINE_LIGHTSCOPEDISPLAYER_H
#define URCHINENGINE_LIGHTSCOPEDISPLAYER_H

#include "Urchin3dEngine.h"
#include "UrchinMapHandler.h"

namespace urchin {

    class LightScopeDisplayer {
        public:
            explicit LightScopeDisplayer(SceneManager*);
            ~LightScopeDisplayer();

            void displayLightScopeFor(const SceneLight*);

        private:
            void cleanCurrentDisplay();

            SceneManager *sceneManager;
            std::vector<GeometryModel *> lightScopeModels;
    };

}

#endif
