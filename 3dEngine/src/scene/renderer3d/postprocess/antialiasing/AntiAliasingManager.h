#ifndef URCHINENGINE_ANTIALIASINGMANAGER_H
#define URCHINENGINE_ANTIALIASINGMANAGER_H

#include <memory>

#include "graphic/shader/model/Shader.h"
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

            void onResize(unsigned int, unsigned int);

            void setQuality(Quality quality);

            void applyAntiAliasing();
        private:
            void loadFxaaShader();

            //properties
            Quality quality;
            unsigned int sceneWidth, sceneHeight;

            //display
            std::unique_ptr<Shader> fxaaShader;
            int texLoc, invSceneSizeLoc;
            std::unique_ptr<GenericRenderer> renderer;
    };

}

#endif
