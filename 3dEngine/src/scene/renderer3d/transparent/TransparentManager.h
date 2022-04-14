#pragma once

#include <memory>

#include <scene/renderer3d/camera/Camera.h>
#include <scene/renderer3d/model/displayer/ModelSetDisplayer.h>
#include <scene/renderer3d/lighting/light/LightManager.h>
#include <api/render/target/RenderTarget.h>
#include <api/render/GenericRenderer.h>

namespace urchin {

    class TransparentManager {
        public:
            TransparentManager(bool, LightManager&);
            ~TransparentManager();

            void onTextureUpdate(const std::shared_ptr<Texture>&);
            void onCameraProjectionUpdate(const Camera&);

            const ModelSetDisplayer& getModelSetDisplayer() const;
            void updateModels(const std::vector<Model*>&) const;
            void removeModel(Model*) const;

            void updateTransparentTextures(std::uint64_t, unsigned int, const Camera&) const;
            void loadTransparentTextures(GenericRenderer&, std::size_t, std::size_t) const;

        private:
            struct ModelTransparentShaderConst {
                uint32_t maxLights;
                float maxEmissiveFactor;
            };

            void createOrUpdateRendering();
            void createOrUpdateTextures();
            void createOrUpdateModelSetDisplayer();

            bool useNullRenderTarget;
            LightManager& lightManager;
            unsigned int sceneWidth;
            unsigned int sceneHeight;
            const Camera* camera;

            std::shared_ptr<Texture> depthTexture;
            std::unique_ptr<RenderTarget> renderTarget;
            std::shared_ptr<Texture> accumulationTexture;
            std::shared_ptr<Texture> revealTexture;

            std::unique_ptr<ModelSetDisplayer> modelSetDisplayer;
    };

}
