#include <cassert>
#include <UrchinCommon.h>

#include "libs/vma/vk_mem_alloc.h"
#include "graphics/api/vulkan/helper/BufferUploadBatch.h"
#include "graphics/api/vulkan/helper/CommandBufferHelper.h"
#include "graphics/api/vulkan/setup/GraphicsSetupService.h"

namespace urchin {

    //static
    thread_local BufferUploadBatch* BufferUploadBatch::activeBatch = nullptr;

    BufferUploadBatch::BufferUploadBatch() {
        if (activeBatch) {
            throw std::runtime_error("Buffer upload batch is already active on this thread");
        }
        activeBatch = this;
    }

    BufferUploadBatch::~BufferUploadBatch() {
        assert(activeBatch == this);
        if (!pendingCopies.empty()) {
            try {
                flush();
            } catch (const std::exception& e) {
                Logger::instance().logError("Buffer upload batch flush failed in destructor: " + std::string(e.what()));
            }
        }
        activeBatch = nullptr;
    }

    BufferUploadBatch* BufferUploadBatch::getActiveBatch() {
        return activeBatch;
    }

    void BufferUploadBatch::addCopy(VkBuffer stagingBuffer, VmaAllocation stagingBufferMemory, VkBuffer dstBuffer, VkDeviceSize size) {
        assert(activeBatch == this);
        pendingCopies.emplace_back(PendingCopy{.stagingBuffer = stagingBuffer, .stagingBufferMemory = stagingBufferMemory, .dstBuffer = dstBuffer, .size = size});
    }

    void BufferUploadBatch::cancelCopies(VkBuffer dstBuffer) {
        std::erase_if(pendingCopies, [dstBuffer](const PendingCopy& pendingCopy) {
            if (pendingCopy.dstBuffer == dstBuffer) {
                destroyStagingBuffers({pendingCopy});
                return true;
            }
            return false;
        });
    }

    void BufferUploadBatch::flush() {
        if (pendingCopies.empty()) {
            return;
        }

        //move the copies out first: CommandBufferHelper::beginSingleTimeCommands flushes the active batch
        std::vector<PendingCopy> copiesToFlush = std::move(pendingCopies);
        pendingCopies.clear();

        try {
            CommandBufferData commandBufferData = CommandBufferHelper::beginSingleTimeCommands("batch copy buffers");
            {
                for (const PendingCopy& pendingCopy : copiesToFlush) {
                    VkBufferCopy copyRegion{};
                    copyRegion.srcOffset = 0;
                    copyRegion.dstOffset = 0;
                    copyRegion.size = pendingCopy.size;
                    vkCmdCopyBuffer(commandBufferData.commandBuffer, pendingCopy.stagingBuffer, pendingCopy.dstBuffer, 1, &copyRegion);
                }
            }
            CommandBufferHelper::endSingleTimeCommands(commandBufferData);
        } catch (...) {
            destroyStagingBuffers(copiesToFlush);
            throw;
        }

        destroyStagingBuffers(copiesToFlush);
    }

    void BufferUploadBatch::destroyStagingBuffers(const std::vector<PendingCopy>& copies) {
        for (const PendingCopy& pendingCopy : copies) {
            vmaDestroyBuffer(GraphicsSetupService::instance().getAllocator(), pendingCopy.stagingBuffer, pendingCopy.stagingBufferMemory);
        }
    }

}
