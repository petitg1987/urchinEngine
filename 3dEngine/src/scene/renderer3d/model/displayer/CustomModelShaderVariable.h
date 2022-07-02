#pragma once

#include <UrchinCommon.h>

#include <scene/renderer3d/model/Model.h>
#include <graphics/api/GraphicsApi.h>
#include <graphics/render/GenericRendererBuilder.h>

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
