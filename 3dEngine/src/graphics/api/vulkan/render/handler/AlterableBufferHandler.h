#pragma once

#include <vector>

#include "graphics/api/vulkan/render/handler/BufferHandler.h"

namespace urchin {

    /**
     * The goal of this class is to encapsulate a unique buffer (BufferHandler). However, if the buffer needs to be updated: the buffer is duplicated.
     * This duplication prevents to update a buffer which is currently used in the graphic pipeline.
     * This mechanism allows to have a minimum memory footprint on the static buffers and support parallelism for the dynamic/updated buffers.
     */
    class AlterableBufferHandler {
        public:
            AlterableBufferHandler();
            ~AlterableBufferHandler();

            void initialize(std::string, BufferHandler::BufferType, BufferHandler::BufferKind, std::size_t, std::size_t, const void* dataPtr);
            void cleanup();

            bool updateData(std::size_t, std::size_t, const void*);

            VkBuffer getBuffer(std::size_t) const;

        private:
            void cleanupBuffers();
            void createDynamicBuffers(std::size_t, const void*);

            static constexpr unsigned int MAX_WARNINGS_LOG = 10;

            bool isInitialized;
            bool isStaticBuffer;
            std::string name;

            BufferHandler::BufferType bufferType;
            std::size_t numFramebuffer;

            std::vector<BufferHandler> buffers;
    };

}
