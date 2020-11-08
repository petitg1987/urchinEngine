#ifndef URCHINENGINE_FOGMANAGER_H
#define URCHINENGINE_FOGMANAGER_H

#include <stack>
#include <memory>

#include "scene/renderer3d/landscape/fog/Fog.h"
#include "graphic/shader/model/Shader.h"

namespace urchin {

    class FogManager {
        public:
            FogManager();

            void pushFog(const std::shared_ptr<Fog> &);
            void popFog();
            std::shared_ptr<const Fog> getCurrentFog() const;

            void loadUniformLocationFor(const std::shared_ptr<Shader> &);
            void loadFog();

        private:
            std::stack<std::shared_ptr<Fog>> fogs;

            std::shared_ptr<Shader> deferredShader;
            int hasFogLoc, fogDensityLoc, fogGradientLoc, fogColorLoc, fogMaxHeightLoc;
    };

}

#endif
