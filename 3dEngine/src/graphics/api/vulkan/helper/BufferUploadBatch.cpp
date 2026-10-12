#include <cassert>

#include "libs/vma/vk_mem_alloc.h"
#include "graphics/api/vulkan/helper/BufferUploadBatch.h"
#include "graphics/api/vulkan/helper/CommandBufferHelper.h"
#include "graphics/api/vulkan/setup/GraphicsSetupService.h"

namespace urchin {

    //static
    bool BufferUploadBatch::batchActive = false;
    std::vector<BufferUploadBatch::PendingCopy> BufferUploadBatch::pendingCopies;

    BufferUploadBatch::BufferUploadBatch() {
        if (batchActive) {
            throw std::runtime_error("Buffer upload batch is already active");
        }
        batchActive = true;
    }

    BufferUploadBatch::~BufferUploadBatch() noexcept(false) {
        assert(batchActive);
        batchActive = false;
        flush();
    }

    bool BufferUploadBatch::isActive() {
        return batchActive;
    }

    void BufferUploadBatch::addCopy(VkBuffer stagingBuffer, VmaAllocation stagingBufferMemory, VkBuffer dstBuffer, VkDeviceSize size) {
        assert(isActive());
        pendingCopies.emplace_back(PendingCopy{.stagingBuffer = stagingBuffer, .stagingBufferMemory = stagingBufferMemory, .dstBuffer = dstBuffer, .size = size});
    }

    void BufferUploadBatch::cancelCopies(VkBuffer dstBuffer) {
        std::erase_if(pendingCopies, [dstBuffer](const PendingCopy& pendingCopy) {
            if (pendingCopy.dstBuffer == dstBuffer) {
                destroyStagingBuffer(pendingCopy);
                return true;
            }
            return false;
        });
    }

    void BufferUploadBatch::flush() {
        if (pendingCopies.empty()) {
            return;
        }

        CommandBufferData commandBufferData = CommandBufferHelper::beginSingleTimeCommands("batch copy buffers");
        {
            for (const PendingCopy& pendingCopy : pendingCopies) {
                VkBufferCopy copyRegion{};
                copyRegion.srcOffset = 0;
                copyRegion.dstOffset = 0;
                copyRegion.size = pendingCopy.size;
                vkCmdCopyBuffer(commandBufferData.commandBuffer, pendingCopy.stagingBuffer, pendingCopy.dstBuffer, 1, &copyRegion);
            }
        }
        CommandBufferHelper::endSingleTimeCommands(commandBufferData);

        for (const PendingCopy& pendingCopy : pendingCopies) {
            destroyStagingBuffer(pendingCopy);
        }
        pendingCopies.clear();
    }

    void BufferUploadBatch::destroyStagingBuffer(const PendingCopy& pendingCopy) {
        vmaDestroyBuffer(GraphicsSetupService::instance().getAllocator(), pendingCopy.stagingBuffer, pendingCopy.stagingBufferMemory);
    }

}
