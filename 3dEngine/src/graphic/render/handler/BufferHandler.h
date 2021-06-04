#pragma once

#include <cstdlib>
#include <vulkan/vulkan.h>

VK_DEFINE_HANDLE(VmaAllocation)

namespace urchin {

    class BufferHandler {
        public:
            enum BufferType {
                VERTEX,
                INDEX,
                UNIFORM
            };
            enum BufferKind {
                STATIC,
                DYNAMIC
            };

            BufferHandler();
            ~BufferHandler();

            void initialize(BufferType, std::size_t, const void* dataPtr = nullptr);
            void cleanup();

            VkBuffer getBuffer() const;
            std::size_t getDataSize() const;
            BufferKind getBufferKind() const;

            bool updateData(std::size_t, const void *);

        private:
            void recreateBuffer(const void *);
            void deleteBuffer();
            void copyBuffer(VkBuffer, VkBuffer, VkDeviceSize) const;
            void updateBuffer(const void *);

            bool isInitialized;

            BufferType bufferType;
            std::size_t dataSize;
            BufferKind bufferKind;

            VkBuffer stagingBuffer; //GPU buffer accessible from CPU
            VmaAllocation stagingBufferMemory;
            VkBuffer buffer; //GPU buffer accessible only from stagingBuffer
            VmaAllocation bufferMemory;
    };

}
