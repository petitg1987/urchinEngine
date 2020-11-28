#ifndef URCHINENGINE_SKYMANAGER_H
#define URCHINENGINE_SKYMANAGER_H

#include "graphic/render/target/RenderTarget.h"
#include "scene/renderer3d/camera/Camera.h"
#include "scene/renderer3d/landscape/sky/Skybox.h"

namespace urchin {

    class SkyManager {
        public:
            explicit SkyManager(const RenderTarget *);

            void onCameraProjectionUpdate(const Camera *);

            void setSkybox(std::unique_ptr<Skybox>);
            const std::unique_ptr<Skybox> &getSkybox() const;

            void display(const Matrix4<float> &, const Point3<float> &);

        private:
            const RenderTarget *renderTarget;

            std::unique_ptr<Skybox> skybox;
            Matrix4<float> projectionMatrix;
    };

}

#endif
