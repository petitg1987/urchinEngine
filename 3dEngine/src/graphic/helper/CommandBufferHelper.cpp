#include <graphic/helper/CommandBufferHelper.h>
#include <graphic/setup/GraphicService.h>

namespace urchin {

    VkCommandBuffer CommandBufferHelper::beginSingleTimeCommands() {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = GraphicService::instance().getAllocateCommandPool();
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(GraphicService::instance().getDevices().getLogicalDevice(), &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        return commandBuffer;
    }

    void CommandBufferHelper::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(GraphicService::instance().getQueues().getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(GraphicService::instance().getQueues().getGraphicsQueue()); //use VkFence instead of vkQueueWaitIdle when multiple simultaneous transfers are required

        vkFreeCommandBuffers(GraphicService::instance().getDevices().getLogicalDevice(), GraphicService::instance().getAllocateCommandPool(), 1, &commandBuffer);
    }

}
