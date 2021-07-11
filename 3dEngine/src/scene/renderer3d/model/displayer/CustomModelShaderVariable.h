#pragma once

#include <UrchinCommon.h>

#include <scene/renderer3d/model/Model.h>
#include <graphic/render/GenericRenderer.h>
#include <graphic/render/GenericRendererBuilder.h>
#include <graphic/render/shader/model/Shader.h>

namespace urchin {

    /**
    * Allow to load custom model shader variables for model displayer
    */
    class CustomModelShaderVariable {
        public:
            virtual ~CustomModelShaderVariable() = default;

            virtual void setupMeshRenderer(const std::shared_ptr<GenericRendererBuilder>&) = 0;
            virtual void loadCustomShaderVariables(GenericRenderer&) = 0;

    };

}
