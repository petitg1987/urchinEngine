#ifndef URCHINENGINE_CUSTOMMODELUNIFORM_H
#define URCHINENGINE_CUSTOMMODELUNIFORM_H

#include "UrchinCommon.h"

#include "scene/renderer3d/model/Model.h"
#include "graphic/shader/model/Shader.h"

namespace urchin {

    /**
    * Allow to load custom model uniform for model displayer
    */
    class CustomModelUniform {
        public:
            virtual ~CustomModelUniform() = default;

            virtual void loadCustomUniforms(const Model *, std::unique_ptr<Shader> &) = 0;

    };

}

#endif
