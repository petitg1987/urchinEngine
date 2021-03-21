#ifndef URCHINENGINE_CUSTOMMODELUNIFORM_H
#define URCHINENGINE_CUSTOMMODELUNIFORM_H

#include "UrchinCommon.h"

#include "scene/renderer3d/model/Model.h"
#include "graphic/render/GenericRenderer.h"
#include "graphic/render/GenericRendererBuilder.h"
#include "graphic/shader/model/Shader.h"

namespace urchin {

    /**
    * Allow to load custom model shader variables for model displayer
    */
    class CustomModelShaderVariable {
        public:
            virtual ~CustomModelShaderVariable() = default;

            virtual void setupMeshRenderer(const std::unique_ptr<GenericRendererBuilder>&) = 0;
            virtual void loadCustomShaderVariables(const std::unique_ptr<GenericRenderer>&) = 0;

    };

}

#endif
