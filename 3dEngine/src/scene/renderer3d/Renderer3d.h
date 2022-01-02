#pragma once

#include <string>
#include <memory>
#include <vector>
#include <UrchinCommon.h>

#include <scene/Renderer.h>
#include <scene/renderer3d/camera/Camera.h>
#include <scene/renderer3d/postprocess/antialiasing/AntiAliasingApplier.h>
#include <scene/renderer3d/postprocess/bloom/BloomEffectApplier.h>
#include <scene/renderer3d/transparent/TransparentManager.h>
#include <scene/renderer3d/lighting/ambientocclusion/AmbientOcclusionManager.h>
#include <scene/renderer3d/lighting/shadow/ShadowManager.h>
#include <scene/renderer3d/model/Model.h>
#include <scene/renderer3d/model/displayer/ModelSetDisplayer.h>
#include <scene/renderer3d/landscape/terrain/TerrainContainer.h>
#include <scene/renderer3d/landscape/fog/FogContainer.h>
#include <scene/renderer3d/landscape/water/WaterContainer.h>
#include <scene/renderer3d/landscape/sky/SkyContainer.h>
#include <scene/renderer3d/ui/UiContainer.h>
#include <resources/geometry/GeometryContainer.h>
#include <resources/geometry/GeometryModel.h>
#include <api/render/shader/model/Shader.h>
#include <api/render/GenericRenderer.h>
#include <api/render/target/ScreenRender.h>
#include <api/render/target/OffscreenRender.h>
#include <api/texture/Texture.h>
#include <texture/render/TextureRenderer.h>

namespace urchin {

    class Renderer3d : public Renderer, public Observer {
        public:
            Renderer3d(RenderTarget&, I18nService&);
            ~Renderer3d() override;

            //scene properties
            void onResize(unsigned int, unsigned int) override;
            void notify(Observable*, int) override;

            //graphics
            const OctreeManager<Model>& getModelOctreeManager() const;
            const ModelSetDisplayer& getModelSetDisplayer() const;
            FogContainer& getFogContainer();
            TerrainContainer& getTerrainContainer();
            WaterContainer& getWaterContainer();
            UiContainer& get3dUiContainer();
            GeometryContainer& getGeometryContainer();
            SkyContainer& getSkyContainer();
            LightManager& getLightManager();
            ShadowManager& getShadowManager();
            void activateShadow(bool);
            AmbientOcclusionManager& getAmbientOcclusionManager();
            void activateAmbientOcclusion(bool);
            TransparentManager& getTransparentManager();
            AntiAliasingApplier& getAntiAliasingApplier();
            BloomEffectApplier& getBloomEffectApplier();
            void activateAntiAliasing(bool);

            //camera
            void setCamera(std::shared_ptr<Camera>);
            Camera* getCamera() const;

            //models
            void addModel(std::shared_ptr<Model>);
            std::shared_ptr<Model> removeModel(Model*);
            void preWarmModels();

            //events
            bool isPaused() const;
            bool onKeyPress(unsigned int) override;
            bool onKeyRelease(unsigned int) override;
            bool onChar(char32_t) override;
            bool onMouseMove(double, double) override;
            bool onScroll(double) override;
            void onDisable() override;

            //scene
            void pause();
            void unpause();
            void prepareRendering(float, unsigned int&) override;

        private:
            struct LightingShaderConst {
                uint32_t maxLights;
                uint32_t maxShadowLights;
                uint32_t numberShadowMaps;
                float shadowMapBias;
                float maxEmissiveFactor;
            };

            void onCameraProjectionUpdate();

            //model
            void updateModelsInFrustum();

            //scene
            void createOrUpdateLightingPass();
            void createOrUpdateLightingShader();
            void updateScene(float);
            void deferredRendering(float);
            void renderDebugSceneData(GeometryContainer&);
            void lightingPassRendering();
            void renderDebugFramebuffers(unsigned int);
            void postUpdateScene();

            //scene properties
            RenderTarget& finalRenderTarget;
            unsigned int sceneWidth, sceneHeight;
            bool paused;
            std::shared_ptr<Camera> camera;

            //deferred rendering
            std::unique_ptr<RenderTarget> deferredRenderTarget;
            OctreeManager<Model> modelOctreeManager;
            ModelSetDisplayer modelSetDisplayer;
            std::shared_ptr<AABBoxModel> debugModelOctree;
            std::vector<Model*> modelsInFrustum;

            FogContainer fogContainer;
            TerrainContainer terrainContainer;
            WaterContainer waterContainer;
            UiContainer uiContainer;
            GeometryContainer geometryContainer;
            SkyContainer skyContainer;
            LightManager lightManager;
            AmbientOcclusionManager ambientOcclusionManager;
            TransparentManager transparentManager;
            ShadowManager shadowManager;

            std::shared_ptr<Texture> diffuseTexture, normalAndAmbientTexture, lightingPassTexture;

            //lighting pass rendering + post process
            std::unique_ptr<RenderTarget> lightingRenderTarget;
            std::unique_ptr<GenericRenderer> lightingRenderer;
            std::unique_ptr<Shader> lightingShader;
            struct {
                alignas(16) Matrix4<float> inverseProjectionViewMatrix;
                alignas(16) Point3<float> viewPosition;
            } positioningData;
            struct {
                alignas(4) bool isShadowActivated;
                alignas(4) bool isAmbientOcclusionActivated;
            } visualOption;
            AntiAliasingApplier antiAliasingApplier;
            bool isAntiAliasingActivated;
            BloomEffectApplier bloomEffectApplier;

            //debug
            bool refreshDebugFramebuffers;
            std::vector<std::unique_ptr<TextureRenderer>> debugFramebuffers;
    };

}
