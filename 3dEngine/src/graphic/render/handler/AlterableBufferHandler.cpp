#include <cassert>

#include <graphic/render/handler/AlterableBufferHandler.h>
#include <graphic/setup/GraphicService.h>

namespace urchin {

    AlterableBufferHandler::AlterableBufferHandler() :
            isInitialized(false),
            isStaticBuffer(true),
            bufferType(BufferHandler::BufferType::VERTEX),
            numFramebuffer(1) {

    }

    AlterableBufferHandler::~AlterableBufferHandler() {
        if (isInitialized) {
            Logger::instance().logWarning("Alterable buffer handler not cleanup before destruction");
            cleanup();
        }
    }

    void AlterableBufferHandler::initialize(BufferHandler::BufferType bufferType, std::size_t numFramebuffer, std::size_t dataSize, const void* dataPtr) {
        assert(!isInitialized);

        this->bufferType = bufferType;
        this->numFramebuffer = numFramebuffer;

        if (dataPtr != nullptr) {
            buffers.resize(1);
            for (auto& buffer : buffers) {
                buffer.initialize(bufferType, dataSize, dataPtr);
            }
        } else {
            createFramebufferBuffers(dataSize, dataPtr);
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
        assert(dataPtr != nullptr);

        bool newBuffersCreated;

        if (isStaticBuffer) {
            vkDeviceWaitIdle(GraphicService::instance().getDevices().getLogicalDevice());

            assert(buffers.size() == 1);
            createFramebufferBuffers(dataSize, dataPtr);
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

    void AlterableBufferHandler::createFramebufferBuffers(std::size_t dataSize, const void* dataPtr) {
        cleanupBuffers();

        buffers.resize(numFramebuffer);
        for (auto& buffer : buffers) {
            buffer.initialize(bufferType, dataSize);

            if (dataPtr != nullptr) {
                buffer.updateData(dataSize, dataPtr);
            }

            assert(buffer.getBufferKind() == BufferHandler::DYNAMIC);
        }

        isStaticBuffer = false;
    }

}
