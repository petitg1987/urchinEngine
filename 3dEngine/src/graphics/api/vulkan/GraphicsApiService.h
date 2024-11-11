#pragma once

#include <graphics/setup/FramebufferSizeRetriever.h>
#include <graphics/setup/SurfaceCreator.h>

namespace urchin {

    class GraphicsApiService {
        public:
            GraphicsApiService(const std::vector<std::string>&, std::unique_ptr<SurfaceCreator>, FramebufferSizeRetriever&);
            ~GraphicsApiService();

            void frameStart(uint32_t) const;
            void frameEnd() const;
    };

}