#include <graphics/api/vulkan/texture/TextureSamplerCache.h>

namespace urchin {

    TextureSamplerCache::~TextureSamplerCache() {
        if (!samplersCache.empty()) {
            Logger::instance().logError("Texture sampler still used before destruction");
        }
    }

    uint64_t TextureSamplerCache::computeKey(TextureParam::ReadMode readMode, TextureParam::ReadQuality readQuality, TextureParam::Anisotropy anisotropy, uint32_t mipLevels) const {
        auto key = (uint64_t)readMode;
        key += (uint64_t)readQuality << 2;
        key += (uint64_t)anisotropy << 4;
        key += (uint64_t)mipLevels << 32;
        return key;
    }

    void TextureSamplerCache::registerTextureSampler(uint64_t key, VkSampler sampler) {
        #ifdef URCHIN_DEBUG
            assert(!samplersCache.contains(key));
        #endif

        SharedSampler sharedSampler = {.useCount=1, .sampler=sampler};
        samplersCache.try_emplace(key, sharedSampler);
    }

    VkSampler TextureSamplerCache::retrieveAndMarkUsed(uint64_t key) {
        auto itFind = samplersCache.find(key);
        if (itFind != samplersCache.end()) {
            itFind->second.useCount++;
            return itFind->second.sampler;
        }
        return VK_NULL_HANDLE;
    }

    bool TextureSamplerCache::markUnused(uint64_t key) {
        auto itFind = samplersCache.find(key);
        if (itFind == samplersCache.end()) {
            throw std::runtime_error("Sampler not found with key: " + std::to_string(key));
        }

        bool destroySampler = --itFind->second.useCount == 0;
        if (destroySampler) {
            samplersCache.erase(itFind);
        }
        return destroySampler;
    }

}