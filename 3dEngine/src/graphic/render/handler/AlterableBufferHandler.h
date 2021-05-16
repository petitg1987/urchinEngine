#ifndef URCHINENGINE_ALTERABLEBUFFERHANDLER_H
#define URCHINENGINE_ALTERABLEBUFFERHANDLER_H

#include <vector>

#include "graphic/render/handler/BufferHandler.h"
#include "graphic/render/model/DataContainer.h"

namespace urchin {

    /**
     * The goal of this class is to encapsulate an unique buffer (BufferHandler). However, if the buffer needs to be updated: the buffer is duplicated.
     * This duplication prevents to update a buffer which is currently used in the graphic pipeline.
     * This mechanism allows to have a minimum memory footprint on the static buffers and support parallelism for the dynamic/updated buffers.
     */
    class AlterableBufferHandler {
        public:
            AlterableBufferHandler();
            ~AlterableBufferHandler();

            void initialize(BufferHandler::BufferType, std::size_t, std::size_t, const void* dataPtr = nullptr);
            void cleanup();

            bool updateData(std::size_t, std::size_t, const void*);

            VkBuffer getBuffer(std::size_t) const;

        private:
            void cleanupBuffers();
            void createFramebufferBuffers(std::size_t, const void*);

            bool isInitialized;
            bool isStaticBuffer;

            BufferHandler::BufferType bufferType;
            std::size_t numFramebuffer;

            std::vector<BufferHandler> buffers;
    };

}

#endif
