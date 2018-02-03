#ifndef URCHINENGINE_FOGMANAGER_H
#define URCHINENGINE_FOGMANAGER_H

#include "scene/renderer3d/fog/Fog.h"

namespace urchin
{

    class FogManager
    {
        public:
            FogManager();

            void activateFog(Fog *);
            void disableFog();
            bool hasFog() const;

            void loadUniformLocationFor(unsigned int);
            void loadFog();

        private:
            Fog *currentFog;
            unsigned int deferredShaderID;
            int hasFogLoc, fogDensityLoc, fogGradientLoc, fogColorLoc, fogMaxHeightLoc;
    };

}

#endif
