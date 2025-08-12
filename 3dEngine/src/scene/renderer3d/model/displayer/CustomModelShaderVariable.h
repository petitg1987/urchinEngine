#pragma once

#include "graphics/api/GraphicsApi.h"
#include "graphics/render/GenericRendererBuilder.h"

namespace urchin {

    /**
    * Allow to load custom model shader variables for model displayer
    */
    class CustomModelShaderVariable {
        public:
            virtual ~CustomModelShaderVariable() = default;

            virtual void setupMeshRenderer(const std::shared_ptr<GenericRendererBuilder>&, uint32_t, uint32_t, uint32_t) = 0;
            virtual void loadCustomShaderVariables(GenericRenderer&, uint32_t, uint32_t, uint32_t) = 0;

    };

}
