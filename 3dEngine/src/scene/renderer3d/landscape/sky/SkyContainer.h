#pragma once

#include "graphics/api/GraphicsApi.h"
#include "scene/renderer3d/landscape/sky/Skybox.h"

namespace urchin {

    class SkyContainer {
        public:
            explicit SkyContainer(RenderTarget&);

            void setSkybox(std::shared_ptr<Skybox>);
            const Skybox* getSkybox() const;
            const std::shared_ptr<Skybox>& getSkyboxPtr() const;

            void prepareRendering(unsigned int, const Matrix4<float>&, const Point3<float>&) const;

        private:
            RenderTarget& renderTarget;

            std::shared_ptr<Skybox> skybox;
    };

}
