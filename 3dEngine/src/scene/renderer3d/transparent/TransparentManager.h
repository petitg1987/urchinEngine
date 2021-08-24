#pragma once

#include <memory>

#include <scene/renderer3d/camera/Camera.h>
#include <scene/renderer3d/model/displayer/ModelSetDisplayer.h>
#include <scene/renderer3d/lighting/light/LightManager.h>
#include <graphic/render/target/RenderTarget.h>
#include <graphic/render/target/OffscreenRender.h>
#include <graphic/render/GenericRenderer.h>

namespace urchin {

    class TransparentManager {
        public:
            explicit TransparentManager(LightManager&);
            ~TransparentManager();

            void onTextureUpdate(const std::shared_ptr<Texture>&);
            void onCameraProjectionUpdate(const Camera&);

            void updateModels(const std::vector<Model*>&);
            void removeModel(Model*);

            void updateTransparentTextures(const Camera&);
            void loadTransparentTextures(GenericRenderer&, std::size_t, std::size_t) const;

        private:
            void createOrUpdateRendering();
            void createOrUpdateTextures();
            void createOrUpdateModelSetDisplayer();

            LightManager& lightManager;
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
