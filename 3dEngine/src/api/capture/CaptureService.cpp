#include <UrchinCommon.h>

#include <libs/vma/vk_mem_alloc.h>
#include <libs/lodepng/lodepng.h>
#include <libs/qoi/qoi.h>
#include <api/capture/CaptureService.h>
#include <api/helper/CommandBufferHelper.h>
#include <api/helper/ImageHelper.h>
#include <api/setup/GraphicService.h>

namespace urchin {

    void CaptureService::takeCapture(const std::string& filename, VkImage srcImage, VkFormat imageFormat, VkImageLayout imageLayout,
                                     unsigned int srcWidth, unsigned int srcHeight, unsigned int width, unsigned int height) const {
        auto logicalDevice = GraphicService::instance().getDevices().getLogicalDevice();
        auto allocator =  GraphicService::instance().getAllocator();

        std::vector<unsigned char> imageData;
        unsigned int dstWidth = (width == 0) ? srcWidth : width;
        unsigned int dstHeight = (height == 0) ? srcHeight : height;
        imageData.resize(dstWidth * dstHeight * 4, 255);

        //create the linear tiled destination image to copy to and to read the memory from
        VmaAllocation imageMemory;
        VkImage dstImage = ImageHelper::createImage(srcWidth, srcHeight, 1, 1, false, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_LINEAR,
                                                    VK_IMAGE_USAGE_TRANSFER_DST_BIT, imageMemory);

        //do the actual blit from the image to our host visible destination image
        VkCommandBuffer copyCmd = CommandBufferHelper::beginSingleTimeCommands();
        {
            //transition destination image to transfer destination layout
            cmdPipelineBarrier(dstImage, copyCmd, 0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

            //transition image to transfer source layout
            cmdPipelineBarrier(srcImage, copyCmd, VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_TRANSFER_READ_BIT, imageLayout, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

            VkImageCopy imageCopyRegion{};
            imageCopyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageCopyRegion.srcSubresource.layerCount = 1;
            imageCopyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageCopyRegion.dstSubresource.layerCount = 1;
            imageCopyRegion.extent.width = srcWidth;
            imageCopyRegion.extent.height = srcHeight;
            imageCopyRegion.extent.depth = 1;

            //issue the copy command
            vkCmdCopyImage(copyCmd, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopyRegion);

            //transition destination image to general layout, which is the required layout for mapping the image memory later on
            cmdPipelineBarrier(dstImage, copyCmd, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);

            //transition back the image after the blit is done
            cmdPipelineBarrier(srcImage, copyCmd, VK_ACCESS_TRANSFER_READ_BIT, VK_ACCESS_MEMORY_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, imageLayout);
        }
        CommandBufferHelper::endSingleTimeCommands(copyCmd);

        //get layout of the image (including row pitch)
        VkImageSubresource subResource { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0 };
        VkSubresourceLayout subResourceLayout;
        vkGetImageSubresourceLayout(logicalDevice, dstImage, &subResource, &subResourceLayout);

        //map image memory so we can start copying from it
        unsigned char* dataDestination;
        vmaMapMemory(allocator, imageMemory, (void**)&dataDestination);
        {
            dataDestination += subResourceLayout.offset;

            std::array<VkFormat, 3> formatsBGRA = {VK_FORMAT_B8G8R8A8_SRGB, VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_B8G8R8A8_SNORM};
            bool bgraToRgba = std::ranges::find(formatsBGRA, imageFormat) != formatsBGRA.end();

            float scaleX = (float)srcWidth / (float)dstWidth;
            float scaleY = (float)srcHeight / (float)dstHeight;

            for (unsigned int y = 0; y < dstHeight; ++y) {
                std::size_t srcIndexY = (unsigned int)(scaleY * ((float)y + 1.0f)) - 1;
                srcIndexY = (std::size_t)MathFunction::clamp((unsigned long)srcIndexY, 0uL, (unsigned long)srcHeight - 1uL);

                for (unsigned int x = 0; x < dstWidth; ++x) {
                    std::size_t srcIndexX = ((unsigned int)(scaleX * ((float)x + 1.0f)) - 1) * 4;
                    srcIndexX = (std::size_t)MathFunction::clamp((unsigned long)srcIndexX, 0uL, (unsigned long)srcWidth * 4uL - 1uL);

                    std::size_t srcIndex = (srcIndexY * subResourceLayout.rowPitch) + srcIndexX;
                    std::size_t dstIndex = (y * dstWidth * 4) + x * 4;

                    if (bgraToRgba) {
                        imageData[dstIndex + 0] = dataDestination[srcIndex + 2];
                        imageData[dstIndex + 1] = dataDestination[srcIndex + 1];
                        imageData[dstIndex + 2] = dataDestination[srcIndex + 0];
                    } else {
                        imageData[dstIndex + 0] = dataDestination[srcIndex + 0];
                        imageData[dstIndex + 1] = dataDestination[srcIndex + 1];
                        imageData[dstIndex + 2] = dataDestination[srcIndex + 2];
                    }
                }
            }
        }
        vmaUnmapMemory(allocator, imageMemory);

        vkDestroyImage(logicalDevice, dstImage, nullptr);
        vmaFreeMemory(allocator, imageMemory);

        if (filename.ends_with(".qoi")) {
            qoi_desc desc;
            desc.channels = 4;
            desc.width = dstWidth;
            desc.height = dstHeight;
            desc.colorspace = 1;
            desc.hasTransparency = false;
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

    void CaptureService::cmdPipelineBarrier(VkImage image, VkCommandBuffer copyCmd, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageLayout oldLayout, VkImageLayout newLayout) const {
        VkImageMemoryBarrier imageMemoryBarrier {};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.srcAccessMask = srcAccessMask;
        imageMemoryBarrier.dstAccessMask = dstAccessMask;
        imageMemoryBarrier.oldLayout = oldLayout;
        imageMemoryBarrier.newLayout = newLayout;
        imageMemoryBarrier.image = image;
        imageMemoryBarrier.subresourceRange = VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

        vkCmdPipelineBarrier(copyCmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
    }

}
