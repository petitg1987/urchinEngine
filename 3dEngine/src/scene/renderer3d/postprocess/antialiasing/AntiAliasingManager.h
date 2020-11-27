#ifndef URCHINENGINE_ANTIALIASINGMANAGER_H
#define URCHINENGINE_ANTIALIASINGMANAGER_H

#include <memory>

#include "graphic/shader/model/Shader.h"
#include "graphic/shader/model/ShaderVar.h"
#include "graphic/render/GenericRenderer.h"
#include "graphic/render/target/TargetRenderer.h"

namespace urchin {

    class AntiAliasingManager {
        public:
            enum Quality {
                LOW = 0,
                MEDIUM,
                HIGH,
                VERY_HIGH
            };

            explicit AntiAliasingManager(const TargetRenderer *);

            void onTextureUpdate(const std::shared_ptr<Texture> &);
            void onResize(unsigned int, unsigned int);

            void setQuality(Quality quality);

            void applyAntiAliasing();

        private:
            void loadFxaaShader();

            //properties
            const TargetRenderer *renderTarget;
            Quality quality;
            unsigned int sceneWidth, sceneHeight;

            //display
            std::unique_ptr<Shader> fxaaShader;
            ShaderVar invSceneSizeShaderVar;
            std::unique_ptr<GenericRenderer> renderer;
    };

}

#endif
