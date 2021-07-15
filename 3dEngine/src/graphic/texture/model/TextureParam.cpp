#include <stdexcept>

#include <graphic/texture/model/TextureParam.h>
#include <graphic/setup/GraphicService.h>

namespace urchin {

    TextureParam::TextureParam(ReadMode readMode, ReadQuality readQuality, Anisotropy anisotropy) :
            isInitialized(false),
            readMode(readMode),
            readQuality(readQuality),
            anisotropy(anisotropy),
            textureSampler(nullptr) {

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
        auto logicalDevice = GraphicService::instance().getDevices().getLogicalDevice();

        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(GraphicService::instance().getDevices().getPhysicalDevice(), &properties);

        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = getVkReadQuality();
        samplerInfo.minFilter = getVkReadQuality();
        samplerInfo.addressModeU = getVkReadMode();
        samplerInfo.addressModeV = getVkReadMode();
        samplerInfo.addressModeW = getVkReadMode();
        samplerInfo.anisotropyEnable = VK_TRUE;
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
            throw std::runtime_error("Failed to create texture sampler with error code: " + std::to_string(result));
        }

        isInitialized = true;
    }

    void TextureParam::cleanup() {
        assert(isInitialized);
        auto logicalDevice = GraphicService::instance().getDevices().getLogicalDevice();

        vkDestroySampler(logicalDevice, textureSampler, nullptr);

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

}
