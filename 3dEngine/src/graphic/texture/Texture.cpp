#include <cassert>

#include "Texture.h"
#include "libs/vma/vk_mem_alloc.h"
#include "graphic/setup/GraphicService.h"
#include "graphic/helper/BufferHelper.h"
#include "graphic/helper/CommandBufferHelper.h"
#include "graphic/helper/ImageHelper.h"

namespace urchin {

    Texture::Texture(TextureType textureType, unsigned int width, unsigned int height, unsigned int layer, TextureFormat format, const std::vector<const void*>& dataPtr) :
            isInitialized(false),
            mipLevels(1),
            writableTexture(false),
            textureType(textureType),
            width(width),
            height(height),
            layer(layer),
            nbImages(dataPtr.size()),
            format(format),
            textureImage(nullptr),
            textureImageMemory(nullptr),
            textureImageView(nullptr) {
        for(const void* imageDataPtr : dataPtr) {
            if(imageDataPtr != nullptr) {
                auto imageDataCharPtr = static_cast<const uint8_t*>(imageDataPtr);
                this->dataPtr.emplace_back(std::vector<uint8_t>(imageDataCharPtr, imageDataCharPtr + getImageSize()));
            } else {
                this->dataPtr.emplace_back(std::vector<uint8_t>(getImageSize(), 0));
            }
        }
    }

    Texture::~Texture() {
        cleanup();
    }

    std::shared_ptr<Texture> Texture::build(unsigned int width, unsigned int height, TextureFormat format, const void* dataPtr) {
        std::vector<const void*> allDataPtr(1, dataPtr);
        return std::shared_ptr<Texture>(new Texture(TextureType::DEFAULT, width, height, 1, format, allDataPtr));
    }

    std::shared_ptr<Texture> Texture::buildArray(unsigned int width, unsigned int height, unsigned int layer, TextureFormat format, const void* dataPtr) {
        assert(layer > 1);
        std::vector<const void*> allDataPtr(1, dataPtr);
        return std::shared_ptr<Texture>(new Texture(TextureType::ARRAY, width, height, layer, format, allDataPtr));
    }

    std::shared_ptr<Texture> Texture::buildCubeMap(unsigned int width, unsigned int height, TextureFormat format, const std::vector<const void*>& cubeDataPtr) {
        assert(cubeDataPtr.size() == 6);
        unsigned int layerCount = 6; //in Vulkan, cube maps are considered as an image of 6 layers
        return std::shared_ptr<Texture>(new Texture(TextureType::CUBE_MAP, width, height, layerCount, format, cubeDataPtr));
    }

    std::shared_ptr<Texture> Texture::buildEmpty() {
        std::vector<const void*> allDataPtr(1, nullptr);
        return std::shared_ptr<Texture>(new Texture(TextureType::DEFAULT, 1, 1, 1, TextureFormat::RGBA_8_INT, allDataPtr));
    }

    std::shared_ptr<Texture> Texture::buildEmptyArray() {
        std::vector<const void*> allDataPtr(1, nullptr);
        return std::shared_ptr<Texture>(new Texture(TextureType::ARRAY, 1, 1, 2, TextureFormat::RGBA_8_INT, allDataPtr));
    }

    void Texture::enableMipmap() {
        assert(!isInitialized);

        mipLevels = (uint32_t)(std::floor(std::log2(std::max(width, height)))) + 1;
    }

    void Texture::enableTextureWriting() {
        assert(!isInitialized);

        writableTexture = true;
    }

    void Texture::initialize() {
        if(!isInitialized) {
            VkImageViewType imageViewType;
            if (textureType == TextureType::DEFAULT) {
                imageViewType = VK_IMAGE_VIEW_TYPE_2D;
            } else if (textureType == TextureType::ARRAY) {
                imageViewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
            } else if (textureType == TextureType::CUBE_MAP) {
                imageViewType = VK_IMAGE_VIEW_TYPE_CUBE;
            } else {
                throw std::runtime_error("Unknown texture type: " + std::to_string(textureType));
            }

            createTextureImage();

            VkImageAspectFlags aspectFlags = isDepthFormat() ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
            textureImageView = ImageHelper::createImageView(textureImage, imageViewType, getVkFormat(), aspectFlags, layer, mipLevels);

            isInitialized = true;
        }
    }

    void Texture::cleanup() {
        if(isInitialized) {
            auto logicalDevice = GraphicService::instance()->getDevices().getLogicalDevice();

            vkDestroyImageView(logicalDevice, textureImageView, nullptr);

            vkDestroyImage(logicalDevice, textureImage, nullptr);
            vmaFreeMemory(GraphicService::instance()->getAllocator(), textureImageMemory);

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

    bool Texture::hasMipmap() const {
        return mipLevels > 1;
    }

    uint32_t Texture::getMipLevels() const {
        return mipLevels;
    }

    VkImageView Texture::getImageView() const {
        assert(isInitialized);
        return textureImageView;
    }

    VkFormat Texture::getVkFormat() const { //should only return format with a coverage of 100%: http://vulkan.gpuinfo.org/listformats.php
        if (format == TextureFormat::DEPTH_32_FLOAT) {
            return VK_FORMAT_D32_SFLOAT;
        } else if (format == TextureFormat::GRAYSCALE_8_INT) {
            return VK_FORMAT_R8_UNORM;
        } else if (format == TextureFormat::GRAYSCALE_16_FLOAT) {
            return VK_FORMAT_R16_SFLOAT;
        } else if (format == TextureFormat::RG_32_FLOAT) {
            return VK_FORMAT_R32G32_SFLOAT;
        } else if (format == TextureFormat::RGBA_8_INT) {
            return VK_FORMAT_R8G8B8A8_UNORM;
        }
        throw std::runtime_error("Unknown texture format: " + std::to_string(format));
    }

    bool Texture::isDepthFormat() const {
        return format == TextureFormat::DEPTH_32_FLOAT;
    }

    void Texture::createTextureImage() {
        auto allocator = GraphicService::instance()->getAllocator();
        VkDeviceSize allImagesSize = getImageSize() * dataPtr.size();

        VmaAllocation stagingBufferMemory;
        VkBuffer stagingBuffer = BufferHelper::createBuffer(allImagesSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBufferMemory);

        void* dataDestination;
        vmaMapMemory(allocator, stagingBufferMemory, &dataDestination);
        {
            for(unsigned int imageIndex = 0; imageIndex < dataPtr.size(); ++imageIndex) {
                void *dataDestinationI = static_cast<uint8_t *>(dataDestination) + (imageIndex * getImageSize());
                memcpy(dataDestinationI, dataPtr[imageIndex].data(), getImageSize());
            }
        }
        vmaUnmapMemory(allocator, stagingBufferMemory);

        bool isCubeMap = textureType == TextureType::CUBE_MAP;
        textureImage = ImageHelper::createImage(width, height, layer, mipLevels, isCubeMap, getVkFormat(), getImageUsage(), textureImageMemory);

        if(!isDepthFormat()) { //depth image layout transition is automatically done in render pass (see 'layout' and 'finalLayout' properties)
            transitionImageLayout(textureImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);
            copyBufferToImage(stagingBuffer, textureImage);
            if (!hasMipmap()) {
                transitionImageLayout(textureImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1);
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
        if(hasMipmap()) {
            usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        }
        if (isDepthFormat()) {
            usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        } else if(writableTexture) {
            usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        }
        return usage;
    }

    void Texture::transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t layoutMipLevels) const {
        VkCommandBuffer commandBuffer = CommandBufferHelper::beginSingleTimeCommands();
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
            } else {
                throw std::invalid_argument("Unsupported layout transition from " + std::to_string(oldLayout) + " to " + std::to_string(newLayout));
            }

            vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage,0,0, nullptr,0,
                                 nullptr,1, &barrier);
        }
        CommandBufferHelper::endSingleTimeCommands(commandBuffer);
    }

    void Texture::copyBufferToImage(VkBuffer buffer, VkImage image) {
        VkCommandBuffer commandBuffer = CommandBufferHelper::beginSingleTimeCommands();
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

            vkCmdCopyBufferToImage(commandBuffer, buffer, image,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, (uint32_t)regions.size(), regions.data());
        }
        CommandBufferHelper::endSingleTimeCommands(commandBuffer);
    }

    void Texture::generateMipmaps(VkImage image, VkFormat imageFormat) const {
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(GraphicService::instance()->getDevices().getPhysicalDevice(), imageFormat, &formatProperties);

        if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
            throw std::runtime_error("Texture image format does not support linear blitting");
        }

        VkCommandBuffer commandBuffer = CommandBufferHelper::beginSingleTimeCommands();
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

                vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                                     0, nullptr, 0, nullptr, 1, &barrier);

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

                vkCmdBlitImage(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                               1, &blit, VK_FILTER_LINEAR);

                barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                                     0, nullptr, 0, nullptr, 1, &barrier);

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

            vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                                 0, nullptr, 0, nullptr, 1, &barrier);
        }
        CommandBufferHelper::endSingleTimeCommands(commandBuffer);
    }

    unsigned int Texture::getBytesByPixel() const {
        if (format == TextureFormat::DEPTH_32_FLOAT || format == TextureFormat::RGBA_8_INT) {
            return 4;
        } else if (format == TextureFormat::GRAYSCALE_8_INT) {
            return 1;
        } else if (format == TextureFormat::GRAYSCALE_16_FLOAT) {
            return 2;
        } else if (format == TextureFormat::RG_32_FLOAT) {
            return 8;
        }
        throw std::runtime_error("Unknown texture format: " + std::to_string(format));
    }

}
