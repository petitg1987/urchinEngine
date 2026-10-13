#include <cstring>

#include <UrchinCommon.h>

#include "libs/vma/vk_mem_alloc.h"
#include "libs/lodepng/lodepng.h"
#include "libs/qoi/qoi.h"
#include "graphics/api/vulkan/capture/CaptureService.h"
#include "graphics/api/vulkan/helper/CommandBufferHelper.h"
#include "graphics/api/vulkan/helper/ImageHelper.h"
#include "graphics/api/vulkan/helper/BufferHelper.h"
#include "graphics/api/vulkan/setup/GraphicsSetupService.h"

namespace urchin {

    CaptureService& CaptureService::instance() {
        static CaptureService instance;
        return instance;
    }

    void CaptureService::takeCapture(const std::string& filename, const Texture& srcTexture, unsigned int width, unsigned int height) const {
        auto allocator = GraphicsSetupService::instance().getAllocator();

        unsigned int srcWidth = srcTexture.getWidth();
        unsigned int srcHeight = srcTexture.getHeight();
        unsigned int dstWidth = (width == 0) ? srcWidth : width;
        unsigned int dstHeight = (height == 0) ? srcHeight : height;
        VkImage srcImage = srcTexture.getImage();
        VkImageLayout srcLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        VkAccessFlags2 srcLayoutWriteAccess = VK_ACCESS_2_NONE;
        VkAccessFlags2 srcLayoutAccess = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT;
        if (srcTexture.getOutputUsage() == OutputUsage::COMPUTE) {
            srcLayout = VK_IMAGE_LAYOUT_GENERAL;
            srcLayoutWriteAccess = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT;
            srcLayoutAccess = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT | VK_ACCESS_2_SHADER_STORAGE_READ_BIT | VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT;
        }
        VkDeviceSize imageDataSize = (VkDeviceSize)dstWidth * dstHeight * 4;

        //create the destination image (blit destination) and the host visible buffer to read the pixels from
        VmaAllocation dstImageMemory;
        VkImage dstImage = ImageHelper::createImage("captureImg", dstWidth, dstHeight, 1, 1, false, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, dstImageMemory);
        VmaAllocation dstBufferMemory;
        VkBuffer dstBuffer = BufferHelper::createBuffer("captureBuffer", imageDataSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, dstBufferMemory);

        CommandBufferData captureCmdData = CommandBufferHelper::beginSingleTimeCommands("captureCmd");
        {
            //transition source image to transfer source layout once all previous commands (rendering) are done
            cmdPipelineBarrier(srcImage, captureCmdData.commandBuffer, VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT, srcLayoutWriteAccess, VK_PIPELINE_STAGE_2_BLIT_BIT, VK_ACCESS_2_TRANSFER_READ_BIT, srcLayout, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

            //transition destination image to transfer destination layout
            cmdPipelineBarrier(dstImage, captureCmdData.commandBuffer, VK_PIPELINE_STAGE_2_NONE, VK_ACCESS_2_NONE, VK_PIPELINE_STAGE_2_BLIT_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

            //blit (format conversion and scaling) the source image into the destination image
            VkImageBlit2 imageBlit{};
            imageBlit.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;
            imageBlit.srcSubresource = VkImageSubresourceLayers{VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
            imageBlit.srcOffsets[1] = VkOffset3D{(int32_t)srcWidth, (int32_t)srcHeight, 1};
            imageBlit.dstSubresource = VkImageSubresourceLayers{VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
            imageBlit.dstOffsets[1] = VkOffset3D{(int32_t)dstWidth, (int32_t)dstHeight, 1};

            VkBlitImageInfo2 blitImageInfo{};
            blitImageInfo.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
            blitImageInfo.srcImage = srcImage;
            blitImageInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            blitImageInfo.dstImage = dstImage;
            blitImageInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            blitImageInfo.regionCount = 1;
            blitImageInfo.pRegions = &imageBlit;
            blitImageInfo.filter = VK_FILTER_LINEAR;
            vkCmdBlitImage2(captureCmdData.commandBuffer, &blitImageInfo);

            //transition back the source image to its original layout
            cmdPipelineBarrier(srcImage, captureCmdData.commandBuffer, VK_PIPELINE_STAGE_2_BLIT_BIT, VK_ACCESS_2_NONE, VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT, srcLayoutAccess, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, srcLayout);

            //copy the destination image into the host visible buffer
            cmdPipelineBarrier(dstImage, captureCmdData.commandBuffer, VK_PIPELINE_STAGE_2_BLIT_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT, VK_PIPELINE_STAGE_2_COPY_BIT, VK_ACCESS_2_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

            VkBufferImageCopy2 bufferImageCopy{};
            bufferImageCopy.sType = VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2;
            bufferImageCopy.bufferOffset = 0;
            bufferImageCopy.bufferRowLength = 0; //tightly packed
            bufferImageCopy.bufferImageHeight = 0; //tightly packed
            bufferImageCopy.imageSubresource = VkImageSubresourceLayers{VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
            bufferImageCopy.imageExtent = VkExtent3D{dstWidth, dstHeight, 1};

            VkCopyImageToBufferInfo2 copyImageToBufferInfo{};
            copyImageToBufferInfo.sType = VK_STRUCTURE_TYPE_COPY_IMAGE_TO_BUFFER_INFO_2;
            copyImageToBufferInfo.srcImage = dstImage;
            copyImageToBufferInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            copyImageToBufferInfo.dstBuffer = dstBuffer;
            copyImageToBufferInfo.regionCount = 1;
            copyImageToBufferInfo.pRegions = &bufferImageCopy;
            vkCmdCopyImageToBuffer2(captureCmdData.commandBuffer, &copyImageToBufferInfo);

            //make the buffer content visible to the host
            VkMemoryBarrier2 hostReadBarrier{};
            hostReadBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2;
            hostReadBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COPY_BIT;
            hostReadBarrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
            hostReadBarrier.dstStageMask = VK_PIPELINE_STAGE_2_HOST_BIT;
            hostReadBarrier.dstAccessMask = VK_ACCESS_2_HOST_READ_BIT;

            VkDependencyInfo dependencyInfo{};
            dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
            dependencyInfo.memoryBarrierCount = 1;
            dependencyInfo.pMemoryBarriers = &hostReadBarrier;
            vkCmdPipelineBarrier2(captureCmdData.commandBuffer, &dependencyInfo);
        }
        CommandBufferHelper::endSingleTimeCommands(captureCmdData);

        std::vector<unsigned char> imageData((std::size_t)imageDataSize);
        unsigned char* bufferData;
        vmaMapMemory(allocator, dstBufferMemory, (void**)&bufferData);
        {
            std::memcpy(imageData.data(), bufferData, imageData.size());
            for (std::size_t i = 3; i < imageData.size(); i += 4) {
                imageData[i] = 255; //scene textures alpha is not relevant
            }
        }
        vmaUnmapMemory(allocator, dstBufferMemory);

        vmaDestroyBuffer(allocator, dstBuffer, dstBufferMemory);
        vmaDestroyImage(allocator, dstImage, dstImageMemory);

        if (filename.ends_with(".qoi")) {
            qoi_desc desc;
            desc.channels = 4;
            desc.width = dstWidth;
            desc.height = dstHeight;
            desc.colorspace = 1;
            desc.minAlpha = 255;
            desc.maxAlpha = 255;
            int createQoiStatus = qoi_write(filename.c_str(), imageData.data(), &desc);
            if (createQoiStatus == 0) {
                throw std::runtime_error("Impossible to encode capture in QOI format (status: " + std::to_string(createQoiStatus) + ", filename: " + filename + ")");
            }
        } else if (filename.ends_with(".png")) {
            unsigned int createPngStatus = lodepng::encode(filename, imageData.data(), dstWidth, dstHeight);
            if (createPngStatus) {
                throw std::runtime_error("Impossible to encode capture in PNG format (status: " + std::to_string(createPngStatus) + ", message: " + lodepng_error_text(createPngStatus) + ", filename: " + filename + ")");
            }
        } else {
            throw std::runtime_error("Unknown filename extension to encode capture: " + filename);
        }
    }

    void CaptureService::cmdPipelineBarrier(VkImage image, VkCommandBuffer commandBuffer, VkPipelineStageFlags2 srcStageMask, VkAccessFlags2 srcAccessMask,
                                            VkPipelineStageFlags2 dstStageMask, VkAccessFlags2 dstAccessMask, VkImageLayout oldLayout, VkImageLayout newLayout) const {
        VkImageMemoryBarrier2 imageMemoryBarrier{};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
        imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.srcAccessMask = srcAccessMask;
        imageMemoryBarrier.dstAccessMask = dstAccessMask;
        imageMemoryBarrier.srcStageMask = srcStageMask;
        imageMemoryBarrier.dstStageMask = dstStageMask;
        imageMemoryBarrier.oldLayout = oldLayout;
        imageMemoryBarrier.newLayout = newLayout;
        imageMemoryBarrier.image = image;
        imageMemoryBarrier.subresourceRange = VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

        VkDependencyInfo dependencyInfo{};
        dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        dependencyInfo.imageMemoryBarrierCount = 1;
        dependencyInfo.pImageMemoryBarriers = &imageMemoryBarrier;

        vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
    }

}
