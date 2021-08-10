#pragma once

#include <memory>

#include <scene/renderer3d/camera/Camera.h>
#include <scene/renderer3d/model/displayer/ModelSetDisplayer.h>
#include <graphic/render/target/RenderTarget.h>
#include <graphic/render/target/OffscreenRender.h>
#include <graphic/render/GenericRenderer.h>

namespace urchin {

    class TransparentManager {
        public:
            TransparentManager();
            ~TransparentManager();

            void onResize(unsigned int, unsigned int);
            void onCameraProjectionUpdate(const Camera&);

            void updateModels(const std::vector<Model*>&);
            void updateTransparentTextures(const Camera&);

        private:
            void createOrUpdateRendering();
            void createOrUpdateTextures();
            void createOrUpdateModelSetDisplayer();

            unsigned int sceneWidth;
            unsigned int sceneHeight;
            const Camera* camera;

            std::unique_ptr<OffscreenRender> offscreenRenderTarget;
            std::shared_ptr<Texture> accumTexture;
            std::shared_ptr<Texture> revealTexture;

            std::unique_ptr<ModelSetDisplayer> modelSetDisplayer;
    };

}
