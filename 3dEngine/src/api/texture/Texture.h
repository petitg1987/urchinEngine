#pragma once

#include <memory>
#include <vector>
#include <vulkan/vulkan.h>
#include <UrchinCommon.h>

#include <api/texture/TextureType.h>
#include <api/texture/TextureFormat.h>

VK_DEFINE_HANDLE(VmaAllocation)

namespace urchin {

    class OffscreenRender;

    class Texture {
        public:
            ~Texture();

            static std::shared_ptr<Texture> build(std::string, unsigned int, unsigned int, TextureFormat, const void*);
            static std::shared_ptr<Texture> buildArray(std::string, unsigned int, unsigned int, unsigned int, TextureFormat, const void*);
            static std::shared_ptr<Texture> buildCubeMap(std::string, unsigned int, unsigned int, TextureFormat, const std::vector<const void*>&);

            static std::shared_ptr<Texture> buildEmptyRgba(std::string);
            static std::shared_ptr<Texture> buildEmptyGreyscale(std::string);
            static std::shared_ptr<Texture> buildEmptyArrayRg(std::string);

            void enableMipmap();
            void enableTextureWriting();

            void initialize();

            TextureType getTextureType() const;
            unsigned int getWidth() const;
            unsigned int getHeight() const;
            unsigned int getLayer() const;
            bool hasMipmap() const;

            uint32_t getMipLevels() const;
            bool isWritableTexture() const;
            void setLastTextureWriter(OffscreenRender*);
            OffscreenRender* getLastTextureWriter() const;
            VkImageView getImageView() const;
            VkFormat getVkFormat() const;

            void takeCapture(const std::string&, unsigned int = 0, unsigned int = 0) const;

        private:
            Texture(std::string, TextureType textureType, unsigned int, unsigned int, unsigned int, TextureFormat, const std::vector<const void*>&);

            void cleanup();

            bool isDepthFormat() const;
            void createTextureImage();
            std::size_t getImageSize() const;
            VkImageUsageFlags getImageUsage() const;
            void transitionImageLayout(VkImage, VkImageLayout, VkImageLayout, uint32_t) const;
            void copyBufferToImage(VkBuffer, VkImage) const;
            void generateMipmaps(VkImage, VkFormat) const;

            unsigned int getBytesByPixel() const;

            bool isInitialized;
            std::string name;

            uint32_t mipLevels;
            bool writableTexture;
            OffscreenRender* lastTextureWriter;

            TextureType textureType;
            unsigned int width;
            unsigned int height;
            unsigned int layer;
            std::size_t nbImages;
            TextureFormat format;
            std::vector<std::vector<uint8_t>> dataPtr;

            VkImage textureImage;
            VmaAllocation textureImageMemory;
            VkImageView textureImageView;
    };

}
