#pragma once

#include <memory>

#include <scene/renderer3d/camera/Camera.h>
#include <scene/renderer3d/model/displayer/ModelSetDisplayer.h>
#include <scene/renderer3d/lighting/light/LightManager.h>
#include <graphics/api/GraphicsApi.h>

namespace urchin {

    class TransparentManager {
        public:
            TransparentManager(bool, LightManager&);

            void onCameraProjectionUpdate(const Camera&);

            void refreshInputTextures(const std::shared_ptr<Texture>&, const std::shared_ptr<Texture>&);
            const ModelSetDisplayer& getModelSetDisplayer() const;
            void replaceAllModels(const std::vector<Model*>&) const;
            void removeModel(Model*) const;

            const std::shared_ptr<Texture>& getOutputTexture() const;
            void drawTransparentModels(uint32_t, unsigned int, const Camera&) const;

        private:
            struct ModelTransparentShaderConst {
                uint32_t maxLights;
                float maxEmissiveFactor;
            };
            struct SortUserData {
                const Camera* camera = nullptr;
                EverGrowHashMap<const Model*, float> modelsDistanceToCamera;
            };

            void createOrUpdateRendering();
            void createOrUpdateTextures();
            void createOrUpdateModelSetDisplayer();

            bool isTestMode;
            LightManager& lightManager;
            unsigned int sceneWidth;
            unsigned int sceneHeight;
            const Camera* camera;

            std::shared_ptr<Texture> depthTexture;
            std::shared_ptr<Texture> sceneTexture;
            std::unique_ptr<OffscreenRender> renderTarget;

            mutable SortUserData sortUserData;
            std::unique_ptr<ModelSetDisplayer> modelSetDisplayer;
    };

}
