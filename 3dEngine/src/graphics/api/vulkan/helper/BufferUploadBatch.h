#pragma once

#include <vector>
#include <vulkan/vulkan.h>

VK_DEFINE_HANDLE(VmaAllocation)

namespace urchin {

    class BufferUploadBatch {
        public:
            BufferUploadBatch();
            ~BufferUploadBatch() noexcept(false);

            BufferUploadBatch(const BufferUploadBatch&) = delete;
            BufferUploadBatch& operator=(const BufferUploadBatch&) = delete;

            static bool isActive();
            static void addCopy(VkBuffer, VmaAllocation, VkBuffer, VkDeviceSize);
            static void cancelCopies(VkBuffer);
            static void flush();

        private:
            struct PendingCopy {
                VkBuffer stagingBuffer;
                VmaAllocation stagingBufferMemory;
                VkBuffer dstBuffer;
                VkDeviceSize size;
            };

            static void destroyStagingBuffer(const PendingCopy&);

            static bool batchActive;
            static std::vector<PendingCopy> pendingCopies;
    };

}
