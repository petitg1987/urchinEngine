#pragma once

#include <vulkan/vulkan.h>
#include <map>

#include "graphics/api/vulkan/texture/TextureParam.h"

namespace urchin {

    struct SharedSampler {
        int useCount;
        VkSampler sampler;
    };

    /**
     * Cache for vkSampler. Cache is required because created samplers on logical device are limited (e.g. 4000 on RTX 2080 Super)
     */
    class TextureSamplerCache {
        public:
            static TextureSamplerCache& instance();
            ~TextureSamplerCache();

            uint64_t computeKey(TextureParam::ReadMode, TextureParam::ReadQuality, TextureParam::Anisotropy, uint32_t) const;
            void registerTextureSampler(uint64_t, VkSampler);

            VkSampler retrieveAndMarkUsed(uint64_t);
            bool markUnused(uint64_t);

        private:
            TextureSamplerCache() = default;

            std::map<uint64_t, SharedSampler> samplersCache;
    };

}