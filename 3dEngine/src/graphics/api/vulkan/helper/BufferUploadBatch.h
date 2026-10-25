#pragma once

#include <vector>
#include <vulkan/vulkan.h>

VK_DEFINE_HANDLE(VmaAllocation)

namespace urchin {

    class BufferUploadBatch {
        public:
            BufferUploadBatch();
            ~BufferUploadBatch();

            BufferUploadBatch(const BufferUploadBatch&) = delete;
            BufferUploadBatch& operator=(const BufferUploadBatch&) = delete;

            static BufferUploadBatch* getActiveBatch();

            void addCopy(VkBuffer, VmaAllocation, VkBuffer, VkDeviceSize);
            void cancelCopies(VkBuffer);
            void flush();

        private:
            struct PendingCopy {
                VkBuffer stagingBuffer;
                VmaAllocation stagingBufferMemory;
                VkBuffer dstBuffer;
                VkDeviceSize size;
            };

            static void destroyStagingBuffers(const std::vector<PendingCopy>&);

            static thread_local BufferUploadBatch* activeBatch;
            std::vector<PendingCopy> pendingCopies;
    };

}
