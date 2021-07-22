#pragma once

#include <graphic/render/target/RenderTarget.h>
#include <scene/renderer3d/camera/Camera.h>
#include <scene/renderer3d/landscape/sky/Skybox.h>

namespace urchin {

    class SkyContainer {
        public:
            explicit SkyContainer(RenderTarget&);

            void onCameraProjectionUpdate(const Camera&);

            void setSkybox(std::unique_ptr<Skybox>);
            const Skybox* getSkybox() const;

            void prepareRendering(const Matrix4<float>&, const Point3<float>&);

        private:
            RenderTarget& renderTarget;

            std::unique_ptr<Skybox> skybox;
            Matrix4<float> projectionMatrix;
    };

}