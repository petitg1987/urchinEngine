#pragma once

#include <api/render/target/RenderTarget.h>
#include <scene/renderer3d/camera/Camera.h>
#include <scene/renderer3d/landscape/sky/Skybox.h>

namespace urchin {

    class SkyContainer {
        public:
            explicit SkyContainer(RenderTarget&);

            void setSkybox(std::shared_ptr<Skybox>);
            const Skybox* getSkybox() const;

            void prepareRendering(unsigned int, const Matrix4<float>&, const Point3<float>&) const;

        private:
            RenderTarget& renderTarget;

            std::shared_ptr<Skybox> skybox;
    };

}
