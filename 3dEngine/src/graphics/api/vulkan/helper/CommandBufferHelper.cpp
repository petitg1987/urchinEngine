#include <libs/vkenum/vk_enum.h>

#include <graphics/api/vulkan/helper/CommandBufferHelper.h>
#include <graphics/api/vulkan/setup/GraphicsSetupService.h>
#include <graphics/api/vulkan/helper/DebugLabelHelper.h>

namespace urchin {

    CommandBufferData CommandBufferHelper::beginSingleTimeCommands(std::string_view name) {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = GraphicsSetupService::instance().getAllocateCommandPool();
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        VkResult resultAllocCmdBuffers = vkAllocateCommandBuffers(GraphicsSetupService::instance().getDevices().getLogicalDevice(), &allocInfo, &commandBuffer);
        if (resultAllocCmdBuffers != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffers with error code '" + std::string(string_VkResult(resultAllocCmdBuffers)) + "' on " + std::string(name));
        }

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        VkResult resultBeginCmdBuffer = vkBeginCommandBuffer(commandBuffer, &beginInfo);
        if (resultBeginCmdBuffer != VK_SUCCESS) {
            throw std::runtime_error("Failed to begin command buffer with error code '" + std::string(string_VkResult(resultBeginCmdBuffer)) + "' on " + std::string(name));
        }

        DebugLabelHelper::nameObject(DebugLabelHelper::COMMAND_BUFFER, commandBuffer, name);

        return CommandBufferData{.commandBuffer = commandBuffer, .name = name};
    }

    void CommandBufferHelper::endSingleTimeCommands(CommandBufferData commandBufferData) {
        VkResult resultEndCmdBuffer = vkEndCommandBuffer(commandBufferData.commandBuffer);
        if (resultEndCmdBuffer != VK_SUCCESS) {
            throw std::runtime_error("Failed to end command buffer with error code '" + std::string(string_VkResult(resultEndCmdBuffer)) + "' on " + std::string(commandBufferData.name));
        }

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBufferData.commandBuffer;

        VkResult resultQueueSubmit = vkQueueSubmit(GraphicsSetupService::instance().getQueues().getGraphicsAndComputeQueue(), 1, &submitInfo, VK_NULL_HANDLE);
        if (resultQueueSubmit != VK_SUCCESS) {
            throw std::runtime_error("Failed to submit queue with error code '" + std::string(string_VkResult(resultQueueSubmit)) + "' on " + std::string(commandBufferData.name));
        }
        VkResult queueWaitIdle = vkQueueWaitIdle(GraphicsSetupService::instance().getQueues().getGraphicsAndComputeQueue()); //use VkFence instead of vkQueueWaitIdle when multiple simultaneous transfers are required
        if (queueWaitIdle != VK_SUCCESS) {
            throw std::runtime_error("Failed to wait for queue idle with error code '" + std::string(string_VkResult(queueWaitIdle)) + "' on " + std::string(commandBufferData.name));
        }

        vkFreeCommandBuffers(GraphicsSetupService::instance().getDevices().getLogicalDevice(), GraphicsSetupService::instance().getAllocateCommandPool(), 1, &commandBufferData.commandBuffer);
    }

}
