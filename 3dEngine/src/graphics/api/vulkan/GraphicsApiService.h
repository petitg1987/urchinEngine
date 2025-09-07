#pragma once

#include "graphics/setup/FramebufferSizeRetriever.h"
#include "graphics/setup/SurfaceCreator.h"
#include "graphics/api/vulkan/MemoryUsage.h"

namespace urchin {

    class GraphicsApiService {
        public:
            GraphicsApiService(const std::vector<std::string>&, std::unique_ptr<SurfaceCreator>, FramebufferSizeRetriever&);
            ~GraphicsApiService();

            void frameStart(uint32_t);
            void frameEnd() const;

            uint64_t getUsedMemory() const;
            uint64_t getTotalMemory() const;

        private:
            MemoryUsage memoryUsage;
    };

}