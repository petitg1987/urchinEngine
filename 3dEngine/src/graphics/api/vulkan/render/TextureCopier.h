#pragma once

#include "graphics/api/vulkan/texture/Texture.h"

namespace urchin {

    class TextureCopier {
        public:
            TextureCopier(Texture&, Texture&);

            void initialize() const;

            void executeCopy(VkCommandBuffer) const;

        private:
            void cmdPipelineBarrierStart(VkImage, VkCommandBuffer, VkAccessFlags, VkAccessFlags, VkImageLayout, VkImageLayout) const;
            void cmdPipelineBarrierEnd(VkImage, VkCommandBuffer, VkAccessFlags, VkImageLayout) const;

            Texture& sourceTexture;
            Texture& destinationTexture;
    };

}