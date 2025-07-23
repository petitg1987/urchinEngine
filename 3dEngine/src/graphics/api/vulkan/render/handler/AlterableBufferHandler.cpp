#include <cassert>

#include "libs/vkenum/vk_enum.h"
#include "graphics/api/vulkan/render/handler/AlterableBufferHandler.h"
#include "graphics/api/vulkan/setup/GraphicsSetupService.h"

namespace urchin {

    AlterableBufferHandler::AlterableBufferHandler() :
            isInitialized(false),
            isStaticBuffer(true),
            bufferType(BufferHandler::BufferType::VERTEX),
            numFramebuffer(1) {

    }

    AlterableBufferHandler::~AlterableBufferHandler() {
        if (isInitialized) {
            static unsigned int numWarningsLogged = 0;
            if (numWarningsLogged++ < MAX_WARNINGS_LOG) {
                Logger::instance().logWarning("Alterable buffer handler not cleanup before destruction");
            }
            cleanup();
        }
    }

    void AlterableBufferHandler::initialize(std::string name, BufferHandler::BufferType bufferType, BufferHandler::BufferKind initialBufferKind, std::size_t numFramebuffer,
                                            std::size_t dataSize, const void* dataPtr) {
        assert(!isInitialized);

        this->name = std::move(name);
        this->bufferType = bufferType;
        this->numFramebuffer = numFramebuffer;

        if (initialBufferKind == BufferHandler::BufferKind::STATIC) {
            if (!dataPtr) {
                throw std::runtime_error("Data must be provided at initialization to build a static buffer");
            }
            buffers.resize(1);
            buffers[0].initialize(this->name, bufferType, initialBufferKind, dataSize, dataPtr);

            isStaticBuffer = true;
        } else if (initialBufferKind == BufferHandler::BufferKind::DYNAMIC) {
            createDynamicBuffers(dataSize, dataPtr);
        } else {
            throw std::runtime_error("Unknown buffer kind: " + std::to_string(initialBufferKind));
        }

        isInitialized = true;
    }

    void AlterableBufferHandler::cleanup() {
        assert(isInitialized);

        cleanupBuffers();

        isInitialized = false;
    }

    void AlterableBufferHandler::cleanupBuffers() {
        for (auto& buffer : buffers) {
            buffer.cleanup();
        }
        buffers.clear();
    }

    bool AlterableBufferHandler::updateData(std::size_t framebufferIndex, std::size_t dataSize, const void* dataPtr) {
        assert(isInitialized);
        assert(dataPtr);

        bool newBuffersCreated;

        if (isStaticBuffer) {
            VkResult result = vkDeviceWaitIdle(GraphicsSetupService::instance().getDevices().getLogicalDevice());
            if (result != VK_SUCCESS) {
                throw std::runtime_error("Failed to wait for device idle with error code: " + std::string(string_VkResult(result)));
            }

            assert(buffers.size() == 1);
            createDynamicBuffers(dataSize, dataPtr);
            newBuffersCreated = true;
        } else {
            assert(buffers.size() > framebufferIndex);
            newBuffersCreated = buffers[framebufferIndex].updateData(dataSize, dataPtr);
        }

        return newBuffersCreated;
    }

    VkBuffer AlterableBufferHandler::getBuffer(std::size_t framebufferIndex) const {
        assert(isInitialized);

        if (isStaticBuffer) {
            assert(buffers.size() == 1);
            return buffers[0].getBuffer();
        }

        assert(buffers.size() > framebufferIndex);
        return buffers[framebufferIndex].getBuffer();
    }

    void AlterableBufferHandler::createDynamicBuffers(std::size_t dataSize, const void* dataPtr) {
        cleanupBuffers();

        buffers.resize(numFramebuffer);
        for (auto& buffer : buffers) {
            buffer.initialize(name, bufferType, BufferHandler::DYNAMIC, dataSize, dataPtr);
        }

        isStaticBuffer = false;
    }

}
