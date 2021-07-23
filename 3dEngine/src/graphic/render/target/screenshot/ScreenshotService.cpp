#include <libs/vma/vk_mem_alloc.h>
#include <UrchinCommon.h>

#include <graphic/render/target/screenshot/ScreenshotService.h>
#include <graphic/helper/CommandBufferHelper.h>
#include <graphic/helper/ImageHelper.h>
#include <graphic/setup/GraphicService.h>

namespace urchin {

    void ScreenshotService::takeScreenshot(const ScreenRender& screenRender, unsigned int width, unsigned int height) const {
        auto logicalDevice = GraphicService::instance().getDevices().getLogicalDevice();
        auto allocator =  GraphicService::instance().getAllocator();

        VkImage srcImage = screenRender.getCurrentImage();

        //create the linear tiled destination image to copy to and to read the memory from
        VmaAllocation imageMemory;
        VkImage dstImage = ImageHelper::createImage(screenRender.getWidth(), screenRender.getHeight(), 1, 1, false, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_LINEAR,
                                                    VK_IMAGE_USAGE_TRANSFER_DST_BIT, imageMemory);

        //do the actual blit from the swap chain image to our host visible destination image
        VkCommandBuffer copyCmd = CommandBufferHelper::beginSingleTimeCommands();

        //transition destination image to transfer destination layout
        cmdPipelineBarrier(dstImage, copyCmd, 0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        //transition swap chain image from present to transfer source layout
        cmdPipelineBarrier(srcImage, copyCmd, VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

        VkImageCopy imageCopyRegion{};
        imageCopyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageCopyRegion.srcSubresource.layerCount = 1;
        imageCopyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageCopyRegion.dstSubresource.layerCount = 1;
        imageCopyRegion.extent.width = screenRender.getWidth();
        imageCopyRegion.extent.height = screenRender.getHeight();
        imageCopyRegion.extent.depth = 1;

        //issue the copy command
        vkCmdCopyImage(copyCmd, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,1,&imageCopyRegion);

        //transition destination image to general layout, which is the required layout for mapping the image memory later on
        cmdPipelineBarrier(dstImage, copyCmd, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);

        //transition back the swap chain image after the blit is done
        cmdPipelineBarrier(srcImage, copyCmd, VK_ACCESS_TRANSFER_READ_BIT, VK_ACCESS_MEMORY_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

        CommandBufferHelper::endSingleTimeCommands(copyCmd);

        //get layout of the image (including row pitch)
        VkImageSubresource subResource { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0 };
        VkSubresourceLayout subResourceLayout;
        vkGetImageSubresourceLayout(logicalDevice, dstImage, &subResource, &subResourceLayout);

        //map image memory so we can start copying from it
        std::vector<unsigned char> imageData;
        imageData.resize(width * height * 4, 255);
        const unsigned char* dataDestination;
        vmaMapMemory(allocator, imageMemory, (void**)&dataDestination);
        {
            dataDestination += subResourceLayout.offset;

            std::array<VkFormat, 3> formatsBGRA = {VK_FORMAT_B8G8R8A8_SRGB, VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_B8G8R8A8_SNORM};
            bool bgraToRgba = std::find(formatsBGRA.begin(), formatsBGRA.end(), screenRender.getImageFormat()) != formatsBGRA.end();

            float scaleX = (float)screenRender.getWidth() / (float)width;
            float scaleY = (float)screenRender.getHeight() / (float)height;

            for (unsigned int y = 0; y < height; ++y) {
                for (unsigned int x = 0; x < width; ++x) {
                    std::size_t dstIndexX = 4 * x;
                    std::size_t dstIndexY = y;
                    std::size_t dstIndex = (dstIndexY * width * 4) + dstIndexX;

                    std::size_t srcIndexX = (unsigned int)(scaleX * ((float)dstIndexX + 4.0f)) - 4;
                    srcIndexX = (std::size_t)MathFunction::clamp(srcIndexX, 0uL, (std::size_t)screenRender.getWidth() * 4uL - 1uL);
                    std::size_t srcIndexY = (unsigned int)(scaleY * ((float)dstIndexY + 1.0f)) - 1;
                    srcIndexY = (std::size_t)MathFunction::clamp(srcIndexY, 0uL, (std::size_t)screenRender.getHeight() - 1uL);
                    std::size_t srcIndex = (srcIndexY * subResourceLayout.rowPitch) + srcIndexX;

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

        unsigned createPngStatus = lodepng::encode("/tmp/screenshot.png", imageData.data(), width, height);
        if (createPngStatus) {
            throw std::runtime_error("Impossible to encode image in png with status " + std::to_string(createPngStatus) + ": " + lodepng_error_text(createPngStatus));
        }

        vmaFreeMemory(allocator, imageMemory);
        vkDestroyImage(logicalDevice, dstImage, nullptr);
    }

    void ScreenshotService::cmdPipelineBarrier(VkImage image, VkCommandBuffer copyCmd, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageLayout oldLayout, VkImageLayout newLayout) const {
        VkImageMemoryBarrier imageMemoryBarrier {};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.srcAccessMask = srcAccessMask;
        imageMemoryBarrier.dstAccessMask = dstAccessMask;
        imageMemoryBarrier.oldLayout = oldLayout;
        imageMemoryBarrier.newLayout = newLayout;
        imageMemoryBarrier.image = image;
        imageMemoryBarrier.subresourceRange = VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
        vkCmdPipelineBarrier(copyCmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0,
                             nullptr, 0, nullptr, 1, &imageMemoryBarrier);
    }

}
