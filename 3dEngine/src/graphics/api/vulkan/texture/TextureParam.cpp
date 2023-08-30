#include <stdexcept>
#include <libs/vkenum/vk_enum.h>

#include <graphics/api/vulkan/texture/TextureParam.h>
#include <graphics/api/vulkan/texture/TextureSamplerCache.h>
#include <graphics/api/vulkan/setup/GraphicsSetupService.h>

namespace urchin {

    TextureParam::TextureParam(ReadMode readMode, ReadQuality readQuality, Anisotropy anisotropy) :
            isInitialized(false),
            readMode(readMode),
            readQuality(readQuality),
            anisotropy(anisotropy),
            textureSampler(nullptr),
            samplerCacheKey(0) {

    }

    TextureParam::~TextureParam() {
        if (isInitialized) {
            Logger::instance().logWarning("Texture parameter not cleanup before destruction");
            cleanup();
        }
    }

    TextureParam TextureParam::buildNearest() {
        return TextureParam(ReadMode::EDGE_CLAMP, ReadQuality::NEAREST, Anisotropy::NO_ANISOTROPY);
    }

    TextureParam TextureParam::buildLinear() {
        return TextureParam(ReadMode::EDGE_CLAMP, ReadQuality::LINEAR, Anisotropy::NO_ANISOTROPY);
    }

    TextureParam TextureParam::buildRepeatNearest() {
        return TextureParam(ReadMode::REPEAT, ReadQuality::NEAREST, Anisotropy::NO_ANISOTROPY);
    }

    TextureParam TextureParam::buildRepeatLinear() {
        return TextureParam(ReadMode::REPEAT, ReadQuality::LINEAR, Anisotropy::NO_ANISOTROPY);
    }

    TextureParam TextureParam::build(ReadMode readMode, ReadQuality readQuality, Anisotropy anisotropy) {
        return TextureParam(readMode, readQuality, anisotropy);
    }

    void TextureParam::initialize(uint32_t mipLevels) {
        assert(!isInitialized);

        samplerCacheKey = TextureSamplerCache::instance().computeKey(readMode, readQuality, anisotropy, mipLevels);
        textureSampler = TextureSamplerCache::instance().retrieveAndMarkUsed(samplerCacheKey);

        if (textureSampler == VK_NULL_HANDLE) {
            auto logicalDevice = GraphicsSetupService::instance().getDevices().getLogicalDevice();

            VkPhysicalDeviceProperties properties{};
            vkGetPhysicalDeviceProperties(GraphicsSetupService::instance().getDevices().getPhysicalDevice(), &properties);

            VkSamplerCreateInfo samplerInfo{};
            samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            samplerInfo.magFilter = getVkReadQuality();
            samplerInfo.minFilter = getVkReadQuality();
            samplerInfo.addressModeU = getVkReadMode();
            samplerInfo.addressModeV = getVkReadMode();
            samplerInfo.addressModeW = getVkReadMode();
            samplerInfo.anisotropyEnable = needAnisotropy() ? VK_TRUE : VK_FALSE;
            samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
            samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
            samplerInfo.unnormalizedCoordinates = VK_FALSE;
            samplerInfo.compareEnable = VK_FALSE; //useful for percentage-closer filtering on shadow maps
            samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
            samplerInfo.mipmapMode = getVkMipmapReadQuality();
            samplerInfo.mipLodBias = 0.0f;
            samplerInfo.minLod = 0.0f;
            samplerInfo.maxLod = static_cast<float>(mipLevels);

            VkResult result = vkCreateSampler(logicalDevice, &samplerInfo, nullptr, &textureSampler);
            if (result != VK_SUCCESS) {
                throw std::runtime_error("Failed to create texture sampler with error code: " + std::string(string_VkResult(result)));
            }

            TextureSamplerCache::instance().registerTextureSampler(samplerCacheKey, textureSampler);
        }

        isInitialized = true;
    }

    void TextureParam::cleanup() {
        assert(isInitialized);

        bool destroySampler = TextureSamplerCache::instance().markUnused(samplerCacheKey);
        if (destroySampler) {
            auto logicalDevice = GraphicsSetupService::instance().getDevices().getLogicalDevice();
            vkDestroySampler(logicalDevice, textureSampler, nullptr);
        }

        isInitialized = false;
    }

    VkSampler TextureParam::getTextureSampler() const {
        assert(isInitialized);
        return textureSampler;
    }

    VkSamplerAddressMode TextureParam::getVkReadMode() const {
        if (readMode == ReadMode::EDGE_CLAMP) {
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        } else if (readMode == ReadMode::REPEAT) {
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        }
        throw std::runtime_error("Unknown texture read mode: " + std::to_string(readMode));
    }

    VkFilter TextureParam::getVkReadQuality() const {
        if (readQuality == ReadQuality::NEAREST) {
            return VK_FILTER_NEAREST;
        } else if (readQuality == ReadQuality::LINEAR) {
            return VK_FILTER_LINEAR;
        }
        throw std::runtime_error("Unknown texture read quality: " + std::to_string(readQuality));
    }

    VkSamplerMipmapMode TextureParam::getVkMipmapReadQuality() const {
        if (readQuality == ReadQuality::NEAREST) {
            return VK_SAMPLER_MIPMAP_MODE_NEAREST;
        } else if (readQuality == ReadQuality::LINEAR) {
            return VK_SAMPLER_MIPMAP_MODE_LINEAR;
        }
        throw std::runtime_error("Unknown texture read quality: " + std::to_string(readQuality));
    }

    bool TextureParam::needAnisotropy() const {
        if (anisotropy == Anisotropy::NO_ANISOTROPY) {
            return false;
        } else if (anisotropy == Anisotropy::ANISOTROPY) {
            return true;
        }
        throw std::runtime_error("Unknown anisotropy type: " + std::to_string(anisotropy));
    }

    bool operator== (const TextureParam& a, const TextureParam& b) {
        return a.readMode == b.readMode && a.readQuality == b.readQuality && a.anisotropy == b.anisotropy;
    }

}
