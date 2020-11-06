#ifndef URCHINENGINE_ANTIALIASINGMANAGER_H
#define URCHINENGINE_ANTIALIASINGMANAGER_H

#include <memory>

#include "graphic/render/GenericRenderer.h"

namespace urchin {

    class AntiAliasingManager {
        public:
            enum Quality
            {
                LOW = 0,
                MEDIUM,
                HIGH,
                VERY_HIGH
            };

            explicit AntiAliasingManager(unsigned int);
            ~AntiAliasingManager();

            void onResize(unsigned int, unsigned int);

            void setQuality(Quality quality);

            void applyAntiAliasing();
        private:
            void loadFxaaShader();

            //properties
            Quality quality;
            unsigned int sceneWidth, sceneHeight;

            //display
            unsigned int fxaaShader;
            int texLoc, invSceneSizeLoc;
            std::shared_ptr<GenericRenderer> renderer;
    };

}

#endif
