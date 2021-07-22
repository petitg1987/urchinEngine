#include <graphic/render/target/screenshot/ScreenshotService.h>
#include <graphic/helper/MemoryHelper.h>
#include <graphic/helper/CommandBufferHelper.h>
#include <graphic/setup/GraphicService.h>

namespace urchin {

    void ScreenshotService::takeScreenshot(const ScreenRender& screenRender) const { //TODO: use vma: vmaBindImageMemory, etc...
        auto logicalDevice = GraphicService::instance().getDevices().getLogicalDevice();
        vkDeviceWaitIdle(logicalDevice);
        VkImage srcImage = screenRender.getCurrentImage();

        //create the linear tiled destination image to copy to and to read the memory from
        VkImageCreateInfo imageCreateCI = {};
        imageCreateCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateCI.imageType = VK_IMAGE_TYPE_2D;
        imageCreateCI.format = VK_FORMAT_R8G8B8A8_UNORM; //vkCmdBlitImage (if supported) will also do format conversions if the swap chain color format would differ
        imageCreateCI.extent.width = screenRender.getWidth();
        imageCreateCI.extent.height = screenRender.getHeight();
        imageCreateCI.extent.depth = 1;
        imageCreateCI.arrayLayers = 1;
        imageCreateCI.mipLevels = 1;
        imageCreateCI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageCreateCI.samples = VK_SAMPLE_COUNT_1_BIT;
        imageCreateCI.tiling = VK_IMAGE_TILING_LINEAR;
        imageCreateCI.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;

        //create the image
        VkImage dstImage;
        VkResult result = vkCreateImage(logicalDevice, &imageCreateCI, nullptr, &dstImage);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create destination image for screenshot with error code: " + std::to_string(result));
        }

        //create memory to back up the image
        VkMemoryRequirements memRequirements;

        VkMemoryAllocateInfo memAllocInfo {};
        memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        VkDeviceMemory dstImageMemory;
        vkGetImageMemoryRequirements(logicalDevice, dstImage, &memRequirements);
        memAllocInfo.allocationSize = memRequirements.size;

        //memory must be host visible to copy from
        memAllocInfo.memoryTypeIndex = MemoryHelper::findMemoryType(memRequirements.memoryTypeBits,  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        result = vkAllocateMemory(logicalDevice, &memAllocInfo, nullptr, &dstImageMemory);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to ... with error code:" + std::to_string(result)); //TODO continue...
        }
        result = vkBindImageMemory(logicalDevice, dstImage, dstImageMemory, 0);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to ... with error code: " + std::to_string(result)); //TODO continue...
        }

        //do the actual blit from the swap chain image to our host visible destination image
        VkCommandBuffer copyCmd = CommandBufferHelper::beginSingleTimeCommands();

        //transition destination image to transfer destination layout
        VkImageMemoryBarrier dstImageMemoryBarrier {};
        dstImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        dstImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        dstImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        dstImageMemoryBarrier.srcAccessMask = 0;
        dstImageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        dstImageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        dstImageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        dstImageMemoryBarrier.image = dstImage;
        dstImageMemoryBarrier.subresourceRange = VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
        vkCmdPipelineBarrier(copyCmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &dstImageMemoryBarrier);

        //transition swap chain image from present to transfer source layout
        VkImageMemoryBarrier srcImageMemoryBarrier {};
        srcImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        srcImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        srcImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        srcImageMemoryBarrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        srcImageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        srcImageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        srcImageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        srcImageMemoryBarrier.image = srcImage;
        srcImageMemoryBarrier.subresourceRange = VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
        vkCmdPipelineBarrier(copyCmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &srcImageMemoryBarrier);

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
        //TODO check result ?

        //transition destination image to general layout, which is the required layout for mapping the image memory later on
        VkImageMemoryBarrier dstImageMemoryBarrier2 {};
        dstImageMemoryBarrier2.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        dstImageMemoryBarrier2.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        dstImageMemoryBarrier2.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        dstImageMemoryBarrier2.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        dstImageMemoryBarrier2.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dstImageMemoryBarrier2.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        dstImageMemoryBarrier2.newLayout = VK_IMAGE_LAYOUT_GENERAL;
        dstImageMemoryBarrier2.image = dstImage;
        dstImageMemoryBarrier2.subresourceRange = VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
        vkCmdPipelineBarrier(copyCmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &dstImageMemoryBarrier2);

        //transition back the swap chain image after the blit is done
        VkImageMemoryBarrier srcImageMemoryBarrier2 {};
        srcImageMemoryBarrier2.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        srcImageMemoryBarrier2.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        srcImageMemoryBarrier2.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        srcImageMemoryBarrier2.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        srcImageMemoryBarrier2.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        srcImageMemoryBarrier2.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        srcImageMemoryBarrier2.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        srcImageMemoryBarrier2.image = srcImage;
        srcImageMemoryBarrier2.subresourceRange = VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
        vkCmdPipelineBarrier(copyCmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &srcImageMemoryBarrier2);

        CommandBufferHelper::endSingleTimeCommands(copyCmd);

        //get layout of the image (including row pitch)
        VkImageSubresource subResource { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0 };
        VkSubresourceLayout subResourceLayout;
        vkGetImageSubresourceLayout(logicalDevice, dstImage, &subResource, &subResourceLayout);

        //map image memory so we can start copying from it
        const char* data;
        vkMapMemory(logicalDevice, dstImageMemory, 0, VK_WHOLE_SIZE, 0, (void**)&data);
        data += subResourceLayout.offset;

        std::ofstream file("/tmp/lol/ppm", std::ios::out | std::ios::binary);

        // ppm header
        file << "P6\n" << screenRender.getWidth() << "\n" << screenRender.getHeight() << "\n" << 255 << "\n";

        // ppm binary pixel data
        for (uint32_t y = 0; y < screenRender.getHeight(); y++) {
            unsigned int *row = (unsigned int*)data;
            for (uint32_t x = 0; x < screenRender.getWidth(); x++) {
                file.write((char*)row, 3);
                row++;
            }
            data += subResourceLayout.rowPitch;
        }
        file.close();

        std::cout << "Screenshot saved to disk" << std::endl;

        //clean up resources
        vkUnmapMemory(logicalDevice, dstImageMemory);
        vkFreeMemory(logicalDevice, dstImageMemory, nullptr);
        vkDestroyImage(logicalDevice, dstImage, nullptr);
    }

}
