#pragma once

#include <vulkan/vulkan.h>
#include <graphics/api/vulkan/render/blend/BlendFactor.h>

namespace urchin {

    class BlendFunction {
        public:
            static BlendFunction build(BlendFactor, BlendFactor, BlendFactor, BlendFactor);
            static BlendFunction buildDefault();
            static BlendFunction buildBlendDisabled();

            BlendFactor getSrcColorFactor() const;
            BlendFactor getDstColorFactor() const;
            BlendFactor getSrcAlphaFactor() const;
            BlendFactor getDstAlphaFactor() const;

            void setupColorBlend(VkPipelineColorBlendAttachmentState& colorBlendAttachment) const;

        private:
            BlendFunction(bool, BlendFactor, BlendFactor, BlendFactor, BlendFactor);

            VkBlendFactor toVkBlenderFactor(BlendFactor) const;

            bool bIsBlendEnabled;

            BlendFactor srcColorFactor;
            BlendFactor dstColorFactor;
            BlendFactor srcAlphaFactor;
            BlendFactor dstAlphaFactor;
    };

}
