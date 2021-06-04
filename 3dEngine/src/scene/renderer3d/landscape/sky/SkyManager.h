#pragma once

#include <graphic/render/target/RenderTarget.h>
#include <scene/renderer3d/camera/Camera.h>
#include <scene/renderer3d/landscape/sky/Skybox.h>

namespace urchin {

    class SkyManager {
        public:
            explicit SkyManager(std::shared_ptr<RenderTarget>);

            void onCameraProjectionUpdate(const Camera*);

            void setSkybox(std::unique_ptr<Skybox>);
            const std::unique_ptr<Skybox>& getSkybox() const;

            void prepareRendering(const Matrix4<float>&, const Point3<float>&);

        private:
            std::shared_ptr<RenderTarget> renderTarget;

            std::unique_ptr<Skybox> skybox;
            Matrix4<float> projectionMatrix;
    };

}
