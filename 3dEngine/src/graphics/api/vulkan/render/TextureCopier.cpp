#include <graphics/api/vulkan/render/TextureCopier.h>

namespace urchin {

    TextureCopier::TextureCopier(Texture& sourceTexture, Texture& destinationTexture) :
            sourceTexture(sourceTexture),
            destinationTexture(destinationTexture) {

    }

    void TextureCopier::initialize() {
        if (!destinationTexture.isWritableTexture()) {
            throw std::runtime_error("Destination texture must be writable: " + destinationTexture.getName());
        }
        destinationTexture.initialize();
    }

    void TextureCopier::executeCopy(VkCommandBuffer cmdBuffer) const {
        cmdPipelineBarrierStart(sourceTexture.getImage(), cmdBuffer, VK_ACCESS_2_SHADER_READ_BIT, VK_ACCESS_2_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
        cmdPipelineBarrierStart(destinationTexture.getImage(), cmdBuffer, VK_ACCESS_2_NONE_KHR, VK_ACCESS_2_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        VkImageCopy imageCopyRegion{};
        imageCopyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageCopyRegion.srcSubresource.layerCount = 1;
        imageCopyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageCopyRegion.dstSubresource.layerCount = 1;
        imageCopyRegion.extent.width = sourceTexture.getWidth();
        imageCopyRegion.extent.height = sourceTexture.getHeight();
        imageCopyRegion.extent.depth = sourceTexture.getLayer();

        vkCmdCopyImage(cmdBuffer, sourceTexture.getImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, destinationTexture.getImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopyRegion);

        cmdPipelineBarrierEnd(destinationTexture.getImage(), cmdBuffer, VK_ACCESS_2_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        cmdPipelineBarrierEnd(sourceTexture.getImage(), cmdBuffer, VK_ACCESS_2_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    }

    void TextureCopier::cmdPipelineBarrierStart(VkImage image, VkCommandBuffer copyCmd, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageLayout oldLayout, VkImageLayout newLayout) const {
        VkImageMemoryBarrier2 imageMemoryBarrier{};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
        imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.srcAccessMask = srcAccessMask;
        imageMemoryBarrier.dstAccessMask = dstAccessMask;
        imageMemoryBarrier.srcStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
        imageMemoryBarrier.dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
        imageMemoryBarrier.oldLayout = oldLayout;
        imageMemoryBarrier.newLayout = newLayout;
        imageMemoryBarrier.image = image;
        imageMemoryBarrier.subresourceRange = VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

        VkDependencyInfo dependencyInfo{};
        dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        dependencyInfo.imageMemoryBarrierCount = 1;
        dependencyInfo.pImageMemoryBarriers = &imageMemoryBarrier;

        vkCmdPipelineBarrier2(copyCmd, &dependencyInfo);
    }

    void TextureCopier::cmdPipelineBarrierEnd(VkImage image, VkCommandBuffer copyCmd, VkAccessFlags srcAccessMask, VkImageLayout oldLayout) const {
        VkImageMemoryBarrier2 imageMemoryBarrier{};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
        imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.srcAccessMask = srcAccessMask;
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
        imageMemoryBarrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
        imageMemoryBarrier.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
        imageMemoryBarrier.oldLayout = oldLayout;
        imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageMemoryBarrier.image = image;
        imageMemoryBarrier.subresourceRange = VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

        VkDependencyInfo dependencyInfo{};
        dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        dependencyInfo.imageMemoryBarrierCount = 1;
        dependencyInfo.pImageMemoryBarriers = &imageMemoryBarrier;

        vkCmdPipelineBarrier2(copyCmd, &dependencyInfo);
    }

}
