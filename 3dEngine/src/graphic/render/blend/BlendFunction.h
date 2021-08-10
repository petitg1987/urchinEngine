#pragma once

#include <vulkan/vulkan.h>

#include <graphic/render/blend/BlendFactor.h>

namespace urchin {

    class BlendFunction {
        public:
            static BlendFunction build(BlendFactor, BlendFactor, BlendFactor, BlendFactor);
            static BlendFunction buildDefault();
            static BlendFunction buildBlendDisabled();

            void setupColorBlend(VkPipelineColorBlendAttachmentState &colorBlendAttachment) const;

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
