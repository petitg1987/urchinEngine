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

            void onTexturesUpdate(const std::shared_ptr<Texture>&);
            void onResize(unsigned int, unsigned int);
            void onCameraProjectionUpdate(const Camera&);

            void updateModels(const std::vector<Model*>&);
            void removeModel(Model*);

            void updateTransparentTextures(const Camera&);
            void loadTransparentTextures(GenericRenderer&, std::size_t, std::size_t) const;

        private:
            void createOrUpdateRendering();
            void createOrUpdateTextures();
            void createOrUpdateModelSetDisplayer();

            unsigned int sceneWidth;
            unsigned int sceneHeight;
            const Camera* camera;

            std::shared_ptr<Texture> depthTexture;
            std::unique_ptr<OffscreenRender> offscreenRenderTarget;
            std::shared_ptr<Texture> accumulationTexture;
            std::shared_ptr<Texture> revealTexture;

            std::unique_ptr<ModelSetDisplayer> modelSetDisplayer;
    };

}
