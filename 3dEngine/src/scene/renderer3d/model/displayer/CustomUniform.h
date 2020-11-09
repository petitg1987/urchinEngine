#ifndef URCHINENGINE_CUSTOMUNIFORM_H
#define URCHINENGINE_CUSTOMUNIFORM_H

#include "UrchinCommon.h"

#include "graphic/shader/model/Shader.h"

namespace urchin {

    /**
    * Allow to load custom uniform for model displayer
    */
    class CustomUniform {
        public:
            virtual ~CustomUniform() = default;

            virtual void loadCustomUniforms(std::unique_ptr<Shader> &) = 0;

    };

}

#endif
