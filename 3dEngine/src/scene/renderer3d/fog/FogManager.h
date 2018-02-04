#ifndef URCHINENGINE_FOGMANAGER_H
#define URCHINENGINE_FOGMANAGER_H

#include <stack>
#include <memory>

#include "scene/renderer3d/fog/Fog.h"

namespace urchin
{

    class FogManager
    {
        public:
            FogManager();

            void pushFog(const std::shared_ptr<Fog> &);
            void popFog();
            std::shared_ptr<const Fog> getCurrentFog() const;

            void loadUniformLocationFor(unsigned int);
            void loadFog();

        private:
            std::stack<std::shared_ptr<Fog>> fogs;

            unsigned int deferredShaderID;
            int hasFogLoc, fogDensityLoc, fogGradientLoc, fogColorLoc, fogMaxHeightLoc;
    };

}

#endif
