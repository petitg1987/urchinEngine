#pragma once

#include <UrchinCommon.h>

#include <graphics/setup/FramebufferSizeRetriever.h>
#include <graphics/setup/SurfaceCreator.h>

namespace urchin {

    class GraphicsApiService : public Singleton<GraphicsApiService> {
        public:
            friend class Singleton<GraphicsApiService>;

            static void enableUniqueSurface();
            static void destroySurface();

            void initialize(const std::vector<std::string>&, std::unique_ptr<SurfaceCreator>, FramebufferSizeRetriever&);

            void frameStart(std::uint32_t) const;
            void frameEnd() const;

        private:
            GraphicsApiService() = default;
    };

}