#include <api/helper/CommandBufferHelper.h>
#include <api/setup/GraphicService.h>

namespace urchin {

    VkCommandBuffer CommandBufferHelper::beginSingleTimeCommands() {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = GraphicService::instance().getAllocateCommandPool();
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        VkResult resultAllocCmdBuffers = vkAllocateCommandBuffers(GraphicService::instance().getDevices().getLogicalDevice(), &allocInfo, &commandBuffer);
        if (resultAllocCmdBuffers != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffers with error code: " + std::to_string(resultAllocCmdBuffers));
        }

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        VkResult resultBeginCmdBuffer = vkBeginCommandBuffer(commandBuffer, &beginInfo);
        if (resultBeginCmdBuffer != VK_SUCCESS) {
            throw std::runtime_error("Failed to begin command buffer with error code: " + std::to_string(resultBeginCmdBuffer));
        }

        return commandBuffer;
    }

    void CommandBufferHelper::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
        VkResult resultEndCmdBuffer = vkEndCommandBuffer(commandBuffer);
        if (resultEndCmdBuffer != VK_SUCCESS) {
            throw std::runtime_error("Failed to end command buffer with error code: " + std::to_string(resultEndCmdBuffer));
        }

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        VkResult resultQueueSubmit = vkQueueSubmit(GraphicService::instance().getQueues().getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
        if (resultQueueSubmit != VK_SUCCESS) {
            throw std::runtime_error("Failed to submit queue with error code: " + std::to_string(resultQueueSubmit));
        }
        VkResult queueWaitIdle = vkQueueWaitIdle(GraphicService::instance().getQueues().getGraphicsQueue()); //use VkFence instead of vkQueueWaitIdle when multiple simultaneous transfers are required
        if (queueWaitIdle != VK_SUCCESS) {
            throw std::runtime_error("Failed to wait for queue idle with error code: " + std::to_string(queueWaitIdle));
        }

        vkFreeCommandBuffers(GraphicService::instance().getDevices().getLogicalDevice(), GraphicService::instance().getAllocateCommandPool(), 1, &commandBuffer);
    }

}
