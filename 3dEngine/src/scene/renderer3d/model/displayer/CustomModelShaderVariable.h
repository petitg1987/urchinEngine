#ifndef URCHINENGINE_CUSTOMMODELUNIFORM_H
#define URCHINENGINE_CUSTOMMODELUNIFORM_H

#include "UrchinCommon.h"

#include "scene/renderer3d/model/Model.h"
#include "graphic/shader/model/Shader.h"

namespace urchin {

    /**
    * Allow to load custom model shader variables for model displayer
    */
    class CustomModelShaderVariable {
        public:
            virtual ~CustomModelShaderVariable() = default;

            virtual void loadCustomShaderVariables(const Model *, std::unique_ptr<Shader> &) = 0;

    };

}

#endif
