#pragma once

#include <vulkan/vulkan.h>

#include <graphic/render/blend/BlendFactor.h>

namespace urchin {

    class BlendFunction {
        public:
            static BlendFunction build(BlendFactor, BlendFactor, BlendFactor, BlendFactor);
            static BlendFunction buildDefault();

            void setupColorBlendAttachment(VkPipelineColorBlendAttachmentState &colorBlendAttachment) const;

        private:
            BlendFunction(BlendFactor, BlendFactor, BlendFactor, BlendFactor);

            VkBlendFactor toVkBlenderFactor(BlendFactor) const;

            BlendFactor srcColorFactor;
            BlendFactor dstColorFactor;

            BlendFactor srcAlphaFactor;
            BlendFactor dstAlphaFactor;
    };

}
