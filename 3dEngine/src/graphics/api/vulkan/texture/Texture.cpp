#include <cassert>
#include <cstring>

#include <libs/vma/vk_mem_alloc.h>
#include <graphics/api/vulkan/texture/Texture.h>
#include <graphics/api/vulkan/setup/GraphicsSetupService.h>
#include <graphics/api/vulkan/helper/BufferHelper.h>
#include <graphics/api/vulkan/helper/CommandBufferHelper.h>
#include <graphics/api/vulkan/helper/ImageHelper.h>
#include <pattern/singleton/Singleton.h>

namespace urchin {

    Texture::Texture(TextureType textureType, unsigned int width, unsigned int height, unsigned int layer, TextureFormat format, const std::vector<const void*>& dataPtr, TextureDataType dataType) :
            isInitialized(false),
            textureType(textureType),
            width(width),
            height(height),
            layer(layer),
            nbImages(dataPtr.size()),
            format(format),
            mipLevels(1),
            writableTexture(false),
            outputUsage(OutputUsage::GRAPHICS),
            lastTextureWriter(nullptr),
            textureImage(nullptr),
            textureImageMemory(nullptr),
            textureImageView(nullptr) {
        if (width == 0 || height == 0 || layer == 0) {
            throw std::runtime_error("Invalid texture size of " + std::to_string(width) + "x" + std::to_string(height) + "x" + std::to_string(layer));
        }

        for (const void* imageDataPtr : dataPtr) {
            if (imageDataPtr != nullptr) {
                if (dataType == TextureDataType::INT_16 && format == TextureFormat::GRAYSCALE_16_FLOAT) {
                    throw std::runtime_error("Conversion of input data (int 16 bits) into texture data (float 16 bits) not implemented");
                } else {
                    auto imageDataCharPtr = static_cast<const uint8_t*>(imageDataPtr);
                    this->dataPtr.emplace_back(imageDataCharPtr, imageDataCharPtr + getImageSize());
                }
            } else {
                std::vector<uint8_t> emptyData(0);
                this->dataPtr.emplace_back(emptyData);
            }
        }
    }

    Texture::~Texture() {
        cleanup();
    }

    std::shared_ptr<Texture> Texture::build(std::string name, unsigned int width, unsigned int height, TextureFormat format, const void* dataPtr, TextureDataType textureDataType) {
        std::vector<const void*> allDataPtr(1, dataPtr);
        auto texture = std::shared_ptr<Texture>(new Texture(TextureType::DEFAULT, width, height, 1, format, allDataPtr, textureDataType));
        texture->setName(std::move(name));
        return texture;
    }

    std::shared_ptr<Texture> Texture::buildArray(std::string name, unsigned int width, unsigned int height, unsigned int layer, TextureFormat format, const void* dataPtr, TextureDataType textureDataType) {
        std::vector<const void*> allDataPtr(1, dataPtr);
        auto texture = std::shared_ptr<Texture>(new Texture(TextureType::ARRAY, width, height, layer, format, allDataPtr, textureDataType));
        texture->setName(std::move(name));
        return texture;
    }

    std::shared_ptr<Texture> Texture::buildCubeMap(std::string name, unsigned int width, unsigned int height, TextureFormat format, const std::vector<const void*>& cubeDataPtr, TextureDataType textureDataType) {
        assert(cubeDataPtr.size() == 6);
        unsigned int layerCount = 6; //in Vulkan, cube maps are considered as an image of 6 layers
        auto texture = std::shared_ptr<Texture>(new Texture(TextureType::CUBE_MAP, width, height, layerCount, format, cubeDataPtr, textureDataType));
        texture->setName(std::move(name));
        return texture;
    }

    std::shared_ptr<Texture> Texture::buildEmptyRgba(std::string name) {
        std::array<uint8_t, 4> textureArrayData = {255, 20, 147, 255}; //pink
        std::vector<const void*> allDataPtr(1, textureArrayData.data());
        auto texture = std::shared_ptr<Texture>(new Texture(TextureType::DEFAULT, 1, 1, 1, TextureFormat::RGBA_8_INT, allDataPtr, TextureDataType::INT_8));
        texture->setName(std::move(name));
        return texture;
    }

    std::shared_ptr<Texture> Texture::buildEmptyGreyscale(std::string name) {
        std::array<uint8_t, 1> textureData = {0};
        std::vector<const void*> allDataPtr(1, textureData.data());
        auto texture = std::shared_ptr<Texture>(new Texture(TextureType::DEFAULT, 1, 1, 1, TextureFormat::GRAYSCALE_8_INT, allDataPtr, TextureDataType::INT_8));
        texture->setName(std::move(name));
        return texture;
    }

    std::shared_ptr<Texture> Texture::buildEmptyArrayRg(std::string name) {
        std::array<float, 4> textureArrayData = {
                0.0f, 0.25f,
                0.5f, 1.0f
        };
        std::vector<const void*> allDataPtr(1, textureArrayData.data());
        auto texture = std::shared_ptr<Texture>(new Texture(TextureType::ARRAY, 1, 1, 2, TextureFormat::RG_32_FLOAT, allDataPtr, TextureDataType::FLOAT_32));
        texture->setName(std::move(name));
        return texture;
    }

    void Texture::enableMipmap() {
        assert(!isInitialized);
        mipLevels = (uint32_t)(std::floor(std::log2(std::max(width, height)))) + 1;
    }

    void Texture::enableTextureWriting(OutputUsage outputUsage) {
        assert(!isInitialized);
        this->writableTexture = true;
        this->outputUsage = outputUsage;
    }

    void Texture::initialize() {
        if (!isInitialized) {
            VkImageViewType imageViewType;
            if (textureType == TextureType::DEFAULT) {
                imageViewType = VK_IMAGE_VIEW_TYPE_2D;
            } else if (textureType == TextureType::ARRAY) {
                imageViewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
            } else if (textureType == TextureType::CUBE_MAP) {
                imageViewType = VK_IMAGE_VIEW_TYPE_CUBE;
            } else {
                throw std::runtime_error("Unknown texture type: " + std::to_string((int)textureType));
            }

            createTextureImage();

            VkImageAspectFlags aspectFlags = isDepthFormat() ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
            textureImageView = ImageHelper::createImageView(textureImage, imageViewType, getVkFormat(), aspectFlags, layer, mipLevels);

            isInitialized = true;
        }
    }

    void Texture::cleanup() {
        if (isInitialized) {
            auto logicalDevice = GraphicsSetupService::instance().getDevices().getLogicalDevice();

            vkDestroyImageView(logicalDevice, textureImageView, nullptr);

            vkDestroyImage(logicalDevice, textureImage, nullptr);
            vmaFreeMemory(GraphicsSetupService::instance().getAllocator(), textureImageMemory);

            isInitialized = false;
        }
    }

    TextureType Texture::getTextureType() const {
        return textureType;
    }

    unsigned int Texture::getWidth() const {
        return width;
    }

    unsigned int Texture::getHeight() const {
        return height;
    }

    unsigned int Texture::getLayer() const {
        return layer;
    }

    TextureFormat Texture::getFormat() const {
        return format;
    }

    bool Texture::isDepthFormat() const {
        return format == TextureFormat::DEPTH_32_FLOAT;
    }

    uint32_t Texture::getMipLevels() const {
        return mipLevels;
    }

    bool Texture::hasMipmap() const {
        return mipLevels > 1;
    }

    void Texture::setHasTransparency(bool bHasTransparency) {
        this->bHasTransparency = std::make_optional<bool>(bHasTransparency);
    }

    bool Texture::hasTransparency() const {
        if (!bHasTransparency.has_value()) {
            throw std::runtime_error("No transparency value defined for texture: " + getName());
        }
        return bHasTransparency.value();
    }

    OutputUsage Texture::getOutputUsage() const {
        return outputUsage;
    }

    void Texture::createTextureImage() {
        auto allocator = GraphicsSetupService::instance().getAllocator();
        VkDeviceSize allImagesSize = getImageSize() * dataPtr.size();

        VmaAllocation stagingBufferMemory;
        VkBuffer stagingBuffer = BufferHelper::createBuffer(getName(), allImagesSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBufferMemory);

        void* dataDestination;
        vmaMapMemory(allocator, stagingBufferMemory, &dataDestination);
        {
            for (unsigned int imageIndex = 0; imageIndex < dataPtr.size(); ++imageIndex) {
                void* dataDestinationStartIndex = static_cast<uint8_t*>(dataDestination) + (imageIndex * getImageSize());
                std::memcpy(dataDestinationStartIndex, dataPtr[imageIndex].data(), dataPtr[imageIndex].size());
            }
        }
        vmaUnmapMemory(allocator, stagingBufferMemory);

        bool isCubeMap = textureType == TextureType::CUBE_MAP;
        textureImage = ImageHelper::createImage(getName(), width, height, layer, mipLevels, isCubeMap, getVkFormat(), VK_IMAGE_TILING_OPTIMAL, getImageUsage(), textureImageMemory);

        if (!isDepthFormat()) { //depth image layout transition is automatically done in render pass (see 'layout' and 'finalLayout' properties)
            transitionImageLayout(textureImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);
            copyBufferToImage(stagingBuffer, textureImage);
            if (!hasMipmap()) {
                VkImageLayout newLayout = outputUsage == OutputUsage::GRAPHICS ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_GENERAL;
                transitionImageLayout(textureImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, newLayout, 1);
            } else {
                generateMipmaps(textureImage, getVkFormat());
            }
        }

        vmaDestroyBuffer(allocator, stagingBuffer, stagingBufferMemory);
        dataPtr.clear();
    }

    std::size_t Texture::getImageSize() const {
        return width * height * getBytesByPixel() * (layer / nbImages);
    }

    VkImageUsageFlags Texture::getImageUsage() const {
        VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT; //for mipmap (if exist) and capture

        if (isDepthFormat()) {
            usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        } else if (writableTexture) {
            usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        }

        if (outputUsage == OutputUsage::COMPUTE) {
            usage |= VK_IMAGE_USAGE_STORAGE_BIT;
        }

        return usage;
    }

    void Texture::transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t layoutMipLevels) const {
        CommandBufferData commandBufferData = CommandBufferHelper::beginSingleTimeCommands("layout transition for: " + getName());
        {
            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.oldLayout = oldLayout;
            barrier.newLayout = newLayout;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = image;
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.levelCount = layoutMipLevels;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = layer;

            VkPipelineStageFlags sourceStage;
            VkPipelineStageFlags destinationStage;

            if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
                barrier.srcAccessMask = 0;
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

                sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_GENERAL) { //general is for compute shader output image
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;

                sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            } else {
                throw std::invalid_argument("Unsupported layout transition from " + std::to_string(oldLayout) + " to " + std::to_string(newLayout));
            }

            vkCmdPipelineBarrier(commandBufferData.commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr,1, &barrier);
        }
        CommandBufferHelper::endSingleTimeCommands(commandBufferData);
    }

    void Texture::copyBufferToImage(VkBuffer buffer, VkImage image) const {
        CommandBufferData commandBufferData = CommandBufferHelper::beginSingleTimeCommands("copy for: " + getName());
        {
            std::vector<VkBufferImageCopy> regions;
            regions.reserve(layer);
            for (unsigned int i = 0; i < layer; i++) {
                VkBufferImageCopy region{};
                region.bufferOffset = width * height * getBytesByPixel() * i;
                region.bufferRowLength = 0; //next two values defined to "0" mean that pixels are tightly packed in memory
                region.bufferImageHeight = 0;
                region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                region.imageSubresource.mipLevel = 0;
                region.imageSubresource.baseArrayLayer = i;
                region.imageSubresource.layerCount = 1;
                region.imageOffset = {0, 0, 0};
                region.imageExtent = {width, height, 1};
                regions.emplace_back(region);
            }

            vkCmdCopyBufferToImage(commandBufferData.commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, (uint32_t)regions.size(), regions.data());
        }
        CommandBufferHelper::endSingleTimeCommands(commandBufferData);
    }

    void Texture::generateMipmaps(VkImage image, VkFormat imageFormat) const {
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(GraphicsSetupService::instance().getDevices().getPhysicalDevice(), imageFormat, &formatProperties);

        if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
            throw std::runtime_error("Texture image format does not support linear blitting: " + std::to_string(imageFormat));
        }

        CommandBufferData commandBufferData = CommandBufferHelper::beginSingleTimeCommands("mipmap for: " + getName());
        {
            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.image = image;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = layer;
            barrier.subresourceRange.levelCount = 1;

            auto mipWidth = (int32_t) width;
            auto mipHeight = (int32_t) height;

            for (uint32_t i = 1; i < mipLevels; i++) {
                barrier.subresourceRange.baseMipLevel = i - 1;
                barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

                vkCmdPipelineBarrier(commandBufferData.commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

                VkImageBlit blit{};
                blit.srcOffsets[0] = {0, 0, 0};
                blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
                blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                blit.srcSubresource.mipLevel = i - 1;
                blit.srcSubresource.baseArrayLayer = 0;
                blit.srcSubresource.layerCount = 1;
                blit.dstOffsets[0] = {0, 0, 0};
                blit.dstOffsets[1] = {mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1};
                blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                blit.dstSubresource.mipLevel = i;
                blit.dstSubresource.baseArrayLayer = 0;
                blit.dstSubresource.layerCount = layer;

                vkCmdBlitImage(commandBufferData.commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

                barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                vkCmdPipelineBarrier(commandBufferData.commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

                if (mipWidth > 1) {
                    mipWidth /= 2;
                }
                if (mipHeight > 1) {
                    mipHeight /= 2;
                }
            }

            barrier.subresourceRange.baseMipLevel = mipLevels - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(commandBufferData.commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
        }
        CommandBufferHelper::endSingleTimeCommands(commandBufferData);
    }

    bool Texture::isWritableTexture() const {
        return writableTexture;
    }

    void Texture::setLastTextureWriter(OffscreenRender* lastTextureWriter) {
        assert(writableTexture);
        this->lastTextureWriter = lastTextureWriter;
    }

    OffscreenRender* Texture::getLastTextureWriter() const {
        return lastTextureWriter;
    }

    unsigned int Texture::getBytesByPixel() const {
        if (format == TextureFormat::DEPTH_32_FLOAT || format == TextureFormat::GRAYSCALE_32_FLOAT || format == TextureFormat::RGBA_8_INT || format == TextureFormat::B10G11R11_FLOAT) {
            return 4;
        } else if (format == TextureFormat::GRAYSCALE_8_INT) {
            return 1;
        } else if (format == TextureFormat::GRAYSCALE_16_FLOAT || format == TextureFormat::RG_8_INT) {
            return 2;
        } else if (format == TextureFormat::RG_32_FLOAT || format == TextureFormat::RGBA_16_FLOAT) {
            return 8;
        } else if (format == TextureFormat::RGBA_32_FLOAT) {
            return 16;
        }
        throw std::runtime_error("Unknown texture format: " + std::to_string((int)format));
    }

    VkImage Texture::getImage() const {
        return textureImage;
    }

    VkImageView Texture::getImageView() const {
        assert(isInitialized);
        return textureImageView;
    }

    VkFormat Texture::getVkFormat() const {
        //This method should only return format with a good coverage/support:
        // - For VK_IMAGE_TILING_OPTIMAL (most of the images): http://vulkan.gpuinfo.org/listoptimaltilingformats.php
        // - For VK_IMAGE_TILING_LINEAR (e.g. screenshot): http://vulkan.gpuinfo.org/listlineartilingformats.php
        //Note: the columns to look at depends on ImageHelper::usageFlagToFeatureFlag (mainly: COLOR_ATTACHMENT & TRANSFER_DST)
        if (format == TextureFormat::DEPTH_32_FLOAT) {
            return VK_FORMAT_D32_SFLOAT;
        } else if (format == TextureFormat::GRAYSCALE_8_INT) {
            return VK_FORMAT_R8_UNORM;
        } else if (format == TextureFormat::GRAYSCALE_16_FLOAT) {
            return VK_FORMAT_R16_SFLOAT;
        } else if (format == TextureFormat::GRAYSCALE_32_FLOAT) {
            return VK_FORMAT_R32_SFLOAT;
        } else if (format == TextureFormat::RG_8_INT) {
            return VK_FORMAT_R8G8_UNORM;
        } else if (format == TextureFormat::RG_32_FLOAT) {
            return VK_FORMAT_R32G32_SFLOAT;
        } else if (format == TextureFormat::B10G11R11_FLOAT) {
            return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
        } else if (format == TextureFormat::RGBA_8_INT) {
            return VK_FORMAT_R8G8B8A8_UNORM;
        } else if (format == TextureFormat::RGBA_16_FLOAT) {
            return VK_FORMAT_R16G16B16A16_SFLOAT;
        } else if (format == TextureFormat::RGBA_32_FLOAT) {
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        }
        throw std::runtime_error("Unknown texture format: " + std::to_string((int)format));
    }

}
