#pragma once

#include <memory>
#include <vector>
#include <vulkan/vulkan.h>
#include <UrchinCommon.h>

#include <graphic/texture/TextureType.h>
#include <graphic/texture/TextureFormat.h>

VK_DEFINE_HANDLE(VmaAllocation)

namespace urchin {

    class OffscreenRender;

    class Texture {
        public:
            ~Texture();

            static std::shared_ptr<Texture> build(unsigned int, unsigned int, TextureFormat, const void*);
            static std::shared_ptr<Texture> buildArray(unsigned int, unsigned int, unsigned int, TextureFormat, const void*);
            static std::shared_ptr<Texture> buildCubeMap(unsigned int, unsigned int, TextureFormat, const std::vector<const void*>&);

            static std::shared_ptr<Texture> buildEmptyRgba();
            static std::shared_ptr<Texture> buildEmptyGreyscale();
            static std::shared_ptr<Texture> buildEmptyArrayRg();

            void enableMipmap();
            void enableClearColor(const Vector4<float>&);
            void enableTextureWriting(OffscreenRender*);
            void removeTextureWriter();

            void initialize();

            TextureType getTextureType() const;
            unsigned int getWidth() const;
            unsigned int getHeight() const;
            unsigned int getLayer() const;
            bool hasMipmap() const;
            bool hasClearColorEnabled() const;

            uint32_t getMipLevels() const;
            const Vector4<float>& getClearColor() const;
            OffscreenRender* getTextureWriter() const;
            VkImageView getImageView() const;
            VkFormat getVkFormat() const;

            void takeCapture(const std::string&, unsigned int = 0, unsigned int = 0) const;

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
            bool clearColorEnabled;
            Vector4<float> clearColor;
            bool writableTexture;
            OffscreenRender* textureWriter;

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
