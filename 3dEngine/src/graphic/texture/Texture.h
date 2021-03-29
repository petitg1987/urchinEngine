#ifndef URCHINENGINE_TEXTURE_H
#define URCHINENGINE_TEXTURE_H

#include <memory>
#include <vector>
#include <vulkan/vulkan.h>

#include "graphic/texture/model/TextureType.h"
#include "graphic/texture/model/TextureFormat.h"

VK_DEFINE_HANDLE(VmaAllocation)

namespace urchin {

    class Texture {
        public:
            ~Texture();

            static std::shared_ptr<Texture> build(unsigned int, unsigned int, TextureFormat, const void*);
            static std::shared_ptr<Texture> buildArray(unsigned int, unsigned int, unsigned int, TextureFormat, const void*);
            static std::shared_ptr<Texture> buildCubeMap(unsigned int, unsigned int, TextureFormat, const std::vector<const void*>&);
            static std::shared_ptr<Texture> buildEmpty();
            static std::shared_ptr<Texture> buildEmptyArray();

            void enableMipmap();
            void enableTextureWriting();

            void initialize();

            TextureType getTextureType() const;
            unsigned int getWidth() const;
            unsigned int getHeight() const;
            unsigned int getLayer() const;
            bool hasMipmap() const;
            uint32_t getMipLevels() const;
            VkImageView getImageView() const;
            VkFormat getVkFormat() const;

        private:
            Texture(TextureType textureType, unsigned int, unsigned int, unsigned int, TextureFormat, const std::vector<const void*>&);

            void cleanup();

            bool isDepthFormat() const;
            void createTextureImage();
            std::size_t getImageSize() const;
            VkImageUsageFlags getImageUsage() const;
            void transitionImageLayout(VkImage, VkImageLayout, VkImageLayout, uint32_t) const;
            void copyBufferToImage(VkBuffer, VkImage);
            void generateMipmaps(VkImage, VkFormat) const;

            unsigned int getBytesByPixel() const;

            bool isInitialized;

            uint32_t mipLevels;
            bool writableTexture;
            TextureType textureType;
            unsigned int width, height, layer;
            std::size_t nbImages;
            TextureFormat format;
            std::vector<std::vector<uint8_t>> dataPtr;

            VkImage textureImage;
            VmaAllocation textureImageMemory;
            VkImageView textureImageView;
    };

}

#endif
