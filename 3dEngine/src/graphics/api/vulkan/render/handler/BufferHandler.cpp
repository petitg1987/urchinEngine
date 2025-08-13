#include <cstring>

#include "libs/vma/vk_mem_alloc.h"
#include "graphics/api/vulkan/render/handler/BufferHandler.h"
#include "graphics/api/vulkan/setup/GraphicsSetupService.h"
#include "graphics/api/vulkan/helper/BufferHelper.h"
#include "graphics/api/vulkan/helper/CommandBufferHelper.h"

namespace urchin {

    BufferHandler::BufferHandler() :
            isInitialized(false),
            bufferType(VERTEX),
            dataSize(0),
            bufferKind(STATIC),
            stagingBuffer(nullptr),
            stagingBufferMemory(nullptr),
            buffer(nullptr),
            bufferMemory(nullptr) {

    }

    BufferHandler::~BufferHandler() {
        if (isInitialized) {
            Logger::instance().logWarning("Buffer handler not cleanup before destruction");
            cleanup();
        }
    }

    void BufferHandler::initialize(std::string name, BufferType bufferType, BufferKind initialBufferKind, std::size_t dataSize, const void* dataPtr) {
        assert(!isInitialized);
        assert(dataSize > 0);

        this->name = std::move(name);
        this->bufferType = bufferType;
        this->bufferKind = initialBufferKind; //static buffer are automatically changed to dynamic buffer in case of update
        this->dataSize = dataSize;

        createOrRefreshBuffers(dataPtr);

        isInitialized = true;
    }

    void BufferHandler::cleanup() {
        assert(isInitialized);

        deleteBuffer();

        isInitialized = false;
    }

    VkBuffer BufferHandler::getBuffer() const {
        assert(isInitialized);
        return buffer;
    }

    std::size_t BufferHandler::getDataSize() const {
        assert(isInitialized);
        return dataSize;
    }

    BufferHandler::BufferKind BufferHandler::getBufferKind() const {
        assert(isInitialized);
        return bufferKind;
    }

    bool BufferHandler::updateData(std::size_t updatedDataSize, const void *updatedDataPtr) {
        assert(isInitialized);
        assert(updatedDataPtr != nullptr);

        bool expandBufferSize = this->dataSize < updatedDataSize;
        this->dataSize = std::max(dataSize, updatedDataSize);

        bool newBufferCreated = false;
        if (bufferKind == STATIC || expandBufferSize) {
            bufferKind = DYNAMIC;
            std::vector<char> emptyDataPtr(dataSize, 0);
            createOrRefreshBuffers(emptyDataPtr.data()); //TODO could access to dataPtr out of bound !
            newBufferCreated = true;
        }

        updateBuffer(updatedDataSize, updatedDataPtr);

        return newBufferCreated;
    }

    void BufferHandler::createOrRefreshBuffers(const void* dataPtr) {
        auto allocator = GraphicsSetupService::instance().getAllocator();
        VkDeviceSize bufferSize = dataSize;

        deleteBuffer();

        VkBufferUsageFlagBits usageType = VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
        if (bufferType == VERTEX) {
            usageType = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        } else if (bufferType == INDEX) {
            usageType = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        } else if (bufferType == UNIFORM) {
            usageType = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        } else if (bufferType == STORAGE_BUFFER) {
            usageType = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        }

        if (bufferKind == STATIC) {
            if (!dataPtr) {
                throw std::runtime_error("Data must be provided at initialization to build a static buffer");
            }
            stagingBuffer = BufferHelper::createBuffer(name, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBufferMemory);

            void *dataDestination;
            vmaMapMemory(allocator, stagingBufferMemory, &dataDestination);
            {
                std::memcpy(dataDestination, dataPtr, dataSize);
            }
            vmaUnmapMemory(allocator, stagingBufferMemory);

            buffer = BufferHelper::createBuffer(name, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | usageType, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, bufferMemory);
            copyBuffer(stagingBuffer, buffer, bufferSize);

            vmaDestroyBuffer(allocator, stagingBuffer, stagingBufferMemory);
        } else if (bufferKind == DYNAMIC) {
            buffer = BufferHelper::createBuffer(name, bufferSize, usageType, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, bufferMemory);

            if (dataPtr) {
                updateBuffer(dataSize, dataPtr);
            }
        } else {
            throw std::runtime_error("Unknown buffer kind: " + std::to_string(bufferKind));
        }
    }

    void BufferHandler::deleteBuffer() {
        auto allocator = GraphicsSetupService::instance().getAllocator();

        if (buffer != nullptr) {
            vmaDestroyBuffer(allocator, buffer, nullptr);
            buffer = nullptr;
        }
        if (bufferMemory != nullptr) {
            vmaFreeMemory(allocator, bufferMemory);
            bufferMemory = nullptr;
        }
    }

    void BufferHandler::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const {
        CommandBufferData commandBufferData = CommandBufferHelper::beginSingleTimeCommands("copy buffer");
        {
            VkBufferCopy copyRegion{};
            copyRegion.srcOffset = 0;
            copyRegion.dstOffset = 0;
            copyRegion.size = size;
            vkCmdCopyBuffer(commandBufferData.commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
        }
        CommandBufferHelper::endSingleTimeCommands(commandBufferData);
    }

    void BufferHandler::updateBuffer(std::size_t updatedDataSize, const void *updatedDataPtr) const {
        void *dataDestination;
        vmaMapMemory(GraphicsSetupService::instance().getAllocator(), bufferMemory, &dataDestination);
        {
            std::memcpy(dataDestination, updatedDataPtr, updatedDataSize); //TODO does it update only few byte to GPU ?
        }
        vmaUnmapMemory(GraphicsSetupService::instance().getAllocator(), bufferMemory);
    }

}
