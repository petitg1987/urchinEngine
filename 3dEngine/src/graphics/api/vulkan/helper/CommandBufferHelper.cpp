#include <graphics/api/vulkan/helper/CommandBufferHelper.h>
#include <graphics/api/vulkan/setup/VulkanService.h>
#include "DebugLabelHelper.h"

namespace urchin {

    CommandBufferData CommandBufferHelper::beginSingleTimeCommands(std::string_view name) {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = VulkanService::instance().getAllocateCommandPool();
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        VkResult resultAllocCmdBuffers = vkAllocateCommandBuffers(VulkanService::instance().getDevices().getLogicalDevice(), &allocInfo, &commandBuffer);
        if (resultAllocCmdBuffers != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffers with error code '" + std::to_string(resultAllocCmdBuffers) + "' on " + std::string(name));
        }

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        VkResult resultBeginCmdBuffer = vkBeginCommandBuffer(commandBuffer, &beginInfo);
        if (resultBeginCmdBuffer != VK_SUCCESS) {
            throw std::runtime_error("Failed to begin command buffer with error code '" + std::to_string(resultBeginCmdBuffer) + "' on " + std::string(name));
        }

        DebugLabelHelper::nameObject(DebugLabelHelper::COMMAND_BUFFER, commandBuffer, name);

        return CommandBufferData{.commandBuffer = commandBuffer, .name = name};
    }

    void CommandBufferHelper::endSingleTimeCommands(CommandBufferData commandBufferData) {
        VkResult resultEndCmdBuffer = vkEndCommandBuffer(commandBufferData.commandBuffer);
        if (resultEndCmdBuffer != VK_SUCCESS) {
            throw std::runtime_error("Failed to end command buffer with error code '" + std::to_string(resultEndCmdBuffer) + "' on " + std::string(commandBufferData.name));
        }

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBufferData.commandBuffer;

        VkResult resultQueueSubmit = vkQueueSubmit(VulkanService::instance().getQueues().getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
        if (resultQueueSubmit != VK_SUCCESS) {
            throw std::runtime_error("Failed to submit queue with error code '" + std::to_string(resultQueueSubmit) + "' on " + std::string(commandBufferData.name));
        }
        VkResult queueWaitIdle = vkQueueWaitIdle(VulkanService::instance().getQueues().getGraphicsQueue()); //use VkFence instead of vkQueueWaitIdle when multiple simultaneous transfers are required
        if (queueWaitIdle != VK_SUCCESS) {
            throw std::runtime_error("Failed to wait for queue idle with error code '" + std::to_string(queueWaitIdle) + "' on " + std::string(commandBufferData.name));
        }

        vkFreeCommandBuffers(VulkanService::instance().getDevices().getLogicalDevice(), VulkanService::instance().getAllocateCommandPool(), 1, &commandBufferData.commandBuffer);
    }

}
