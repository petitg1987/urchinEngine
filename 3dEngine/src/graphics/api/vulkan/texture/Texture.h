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

    enum class TextureDataType {
        NULL_PTR,
        INT_8,
        INT_16,
        FLOAT_16,
        FLOAT_32
    };

    enum class OutputUsage {
        GRAPHICS,
        COMPUTE
    };

    class Texture final : public Resource {
        public:
            friend class PipelineProcessor;
            friend class RenderTarget;
            friend class OffscreenRender;
            friend class ScreenRender;

            ~Texture() override;

            static std::shared_ptr<Texture> build(std::string, unsigned int, unsigned int, TextureFormat, const void* = nullptr, TextureDataType = TextureDataType::NULL_PTR);
            static std::shared_ptr<Texture> buildArray(std::string, unsigned int, unsigned int, unsigned int, TextureFormat, const void* = nullptr, TextureDataType = TextureDataType::NULL_PTR);
            static std::shared_ptr<Texture> buildCubeMap(std::string, unsigned int, unsigned int, TextureFormat, const std::vector<const void*>&, TextureDataType);

            static std::shared_ptr<Texture> buildEmptyRgba(std::string);
            static std::shared_ptr<Texture> buildEmptyGreyscale(std::string);
            static std::shared_ptr<Texture> buildEmptyArrayRg(std::string);

            void enableMipmap();
            void enableTextureWriting(OutputUsage);
            void copyTo(Texture&);

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
            OutputUsage getOutputUsage() const;

        private:
            Texture(TextureType textureType, unsigned int, unsigned int, unsigned int, TextureFormat, const std::vector<const void*>&, TextureDataType);

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
            VkImage getImage() const;
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
            OutputUsage outputUsage;

            OffscreenRender* lastTextureWriter;
            VkImage textureImage;
            VmaAllocation textureImageMemory;
            VkImageView textureImageView;
    };

}
