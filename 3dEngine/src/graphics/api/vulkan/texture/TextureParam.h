#pragma once

#include <vulkan/vulkan.h>

namespace urchin {

    class TextureParam {
        public:
            friend class GenericRenderer;

            enum ReadMode {
                EDGE_CLAMP,
                REPEAT
            };

            enum ReadQuality {
                NEAREST,
                LINEAR
            };

            enum Anisotropy {
                NO_ANISOTROPY,
                ANISOTROPY
            };

            ~TextureParam();
            TextureParam(TextureParam&&) = default;
            TextureParam(const TextureParam&) = delete; //delete copy constructor to avoid handling of TextureSamplerCache#SharedSampler#useCount
            TextureParam& operator=(const TextureParam&) = delete; //delete assign operator to avoid handling of TextureSamplerCache#SharedSampler#useCount

            static TextureParam buildNearest();
            static TextureParam buildLinear();
            static TextureParam buildRepeatNearest();
            static TextureParam buildRepeatLinear();
            static TextureParam build(ReadMode, ReadQuality, Anisotropy);

            void initialize(uint32_t);
            void cleanup();

            friend bool operator== (const TextureParam&, const TextureParam&);

        private:
            TextureParam(ReadMode, ReadQuality, Anisotropy);

            VkSampler getTextureSampler() const;
            VkSamplerAddressMode getVkReadMode() const;
            VkFilter getVkReadQuality() const;
            VkSamplerMipmapMode getVkMipmapReadQuality() const;

            bool needAnisotropy() const;

            bool isInitialized;

            ReadMode readMode;
            ReadQuality readQuality;
            Anisotropy anisotropy;

            VkSampler textureSampler;
            uint64_t samplerCacheKey;
    };

}
