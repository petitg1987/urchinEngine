#ifndef URCHINENGINE_CUSTOMUNIFORM_H
#define URCHINENGINE_CUSTOMUNIFORM_H

#include "UrchinCommon.h"

#include "graphic/shader/model/Shader.h"

namespace urchin {

    /**
    * Allow to load custom shader variables for model displayer
    */
    class CustomShaderVariable {
        public:
            virtual ~CustomShaderVariable() = default;

            virtual void loadCustomShaderVariables() = 0;

    };

}

#endif
