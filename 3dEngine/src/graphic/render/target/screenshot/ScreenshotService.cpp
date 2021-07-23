#include <libs/vma/vk_mem_alloc.h>

#include <graphic/render/target/screenshot/ScreenshotService.h>
#include <graphic/helper/CommandBufferHelper.h>
#include <graphic/helper/ImageHelper.h>
#include <graphic/setup/GraphicService.h>

namespace urchin {

    void ScreenshotService::takeScreenshot(const ScreenRender& screenRender) const {
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
        const char* data;
        vmaMapMemory(allocator, imageMemory, (void**)&data);
        {
            data += subResourceLayout.offset;

            std::ofstream file("/tmp/screenshot", std::ios::out | std::ios::binary);

            std::array<VkFormat, 3> formatsBGR = {VK_FORMAT_B8G8R8A8_SRGB, VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_B8G8R8A8_SNORM};
            bool colorSwizzle = (std::find(formatsBGR.begin(), formatsBGR.end(), screenRender.getImageFormat()) != formatsBGR.end());

            // ppm header
            file << "P6\n" << screenRender.getWidth() << "\n" << screenRender.getHeight() << "\n" << 255 << "\n";

            // ppm binary pixel data
            for (uint32_t y = 0; y < screenRender.getHeight(); y++) {
                auto *row = (unsigned int *) data;
                for (uint32_t x = 0; x < screenRender.getWidth(); x++) {
                    if (colorSwizzle) {
                        file.write((char *) row + 2, 1);
                        file.write((char *) row + 1, 1);
                        file.write((char *) row, 1);
                    } else {
                        file.write((char *) row, 3);
                    }
                    row++;
                }
                data += subResourceLayout.rowPitch;
            }
            file.close();

            std::cout << "Screenshot saved to disk" << std::endl;
        }
        vmaUnmapMemory(allocator, imageMemory);

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
