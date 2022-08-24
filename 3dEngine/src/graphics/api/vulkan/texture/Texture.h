#pragma once

#include <memory>
#include <vector>
#include <optional>
#include <vulkan/vulkan.h>
#include <UrchinCommon.h>

#include <graphics/texture/TextureType.h>
#include <graphics/texture/TextureFormat.h>
#include <resources/Resource.h>

VK_DEFINE_HANDLE(VmaAllocation)

namespace urchin {

    class OffscreenRender;

    class Texture final : public Resource {
        public:
            friend class GenericRenderer;
            friend class RenderTarget;
            friend class OffscreenRender;
            friend class ScreenRender;

            ~Texture() override;

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
            TextureFormat getFormat() const;
            bool isDepthFormat() const;
            uint32_t getMipLevels() const;
            bool hasMipmap() const;
            void setHasTransparency(bool);
            bool hasTransparency() const;

            void takeCapture(const std::string&, unsigned int = 0, unsigned int = 0) const;

        private:
            Texture(TextureType textureType, unsigned int, unsigned int, unsigned int, TextureFormat, const std::vector<const void*>&);

            void cleanup();

            void createTextureImage();
            std::size_t getImageSize() const;
            VkImageUsageFlags getImageUsage() const;
            void transitionImageLayout(VkImage, VkImageLayout, VkImageLayout, uint32_t) const;
            void copyBufferToImage(VkBuffer, VkImage) const;
            void generateMipmaps(VkImage, VkFormat) const;

            bool isWritableTexture() const;
            void setLastTextureWriter(OffscreenRender*);
            OffscreenRender* getLastTextureWriter() const;

            unsigned int getBytesByPixel() const;
            VkImageView getImageView() const;
            VkFormat getVkFormat() const;

            bool isInitialized;

            TextureType textureType;
            unsigned int width;
            unsigned int height;
            unsigned int layer;
            std::size_t nbImages;
            TextureFormat format;
            std::vector<std::vector<uint8_t>> dataPtr;
            uint32_t mipLevels;
            std::optional<bool> bHasTransparency;
            bool writableTexture;

            OffscreenRender* lastTextureWriter;
            VkImage textureImage;
            VmaAllocation textureImageMemory;
            VkImageView textureImageView;
    };

}
