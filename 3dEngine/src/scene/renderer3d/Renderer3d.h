#pragma once

#include <string>
#include <memory>
#include <vector>
#include <UrchinCommon.h>

#include <scene/Renderer.h>
#include <scene/renderer3d/VisualConfig.h>
#include <scene/renderer3d/camera/Camera.h>
#include <scene/renderer3d/postprocess/antialiasing/AntiAliasingApplier.h>
#include <scene/renderer3d/postprocess/bloom/BloomEffectApplier.h>
#include <scene/renderer3d/transparent/TransparentManager.h>
#include <scene/renderer3d/lighting/ambientocclusion/AmbientOcclusionManager.h>
#include <scene/renderer3d/lighting/shadow/ShadowManager.h>
#include <scene/renderer3d/model/Model.h>
#include <scene/renderer3d/model/displayer/ModelSetDisplayer.h>
#include <scene/renderer3d/model/culler/ModelOcclusionCuller.h>
#include <scene/renderer3d/landscape/terrain/TerrainContainer.h>
#include <scene/renderer3d/landscape/fog/FogContainer.h>
#include <scene/renderer3d/landscape/water/WaterContainer.h>
#include <scene/renderer3d/landscape/sky/SkyContainer.h>
#include <scene/renderer3d/ui/UiContainer.h>
#include <resources/geometry/GeometryContainer.h>
#include <resources/geometry/GeometryModel.h>
#include <graphics/api/GraphicsApi.h>
#include <texture/render/TextureRenderer.h>

namespace urchin {

    class Renderer3d final : public Renderer, public Observer {
        public:
            Renderer3d(float, RenderTarget&, std::shared_ptr<Camera>, const VisualConfig&, I18nService&);
            ~Renderer3d() override;

            //scene properties
            void onResize(unsigned int, unsigned int) override;
            void notify(Observable*, int) override;

            //graphics
            const ModelOcclusionCuller& getModelOcclusionCuller() const;
            const ModelSetDisplayer& getModelSetDisplayer() const;
            FogContainer& getFogContainer();
            TerrainContainer& getTerrainContainer();
            WaterContainer& getWaterContainer();
            UiContainer& get3dUiContainer();
            GeometryContainer& getGeometryContainer();
            SkyContainer& getSkyContainer();
            LightManager& getLightManager();
            ShadowManager& getShadowManager();
            void updateRenderingScale(float);
            void activateShadow(bool);
            bool isShadowActivated() const;
            AmbientOcclusionManager& getAmbientOcclusionManager();
            void activateAmbientOcclusion(bool);
            bool isAmbientOcclusionActivated() const;
            TransparentManager& getTransparentManager();
            AntiAliasingApplier& getAntiAliasingApplier();
            BloomEffectApplier& getBloomEffectApplier();
            void activateAntiAliasing(bool);
            void applyUpdatedGammaFactor() override;

            //camera
            Camera& getCamera() const;

            //models
            void addModel(std::shared_ptr<Model>);
            std::shared_ptr<Model> removeModel(Model*);
            void preWarmModels();

            //events
            bool isPaused() const;
            bool onKeyPress(InputDeviceKey) override;
            bool onKeyRelease(InputDeviceKey) override;
            bool onChar(char32_t) override;
            bool onMouseMove(double, double, double, double) override;
            bool onScroll(double) override;
            void onDisable() override;

            //scene
            void pause();
            void unpause();
            void prepareRendering(std::uint32_t, float, unsigned int&) override;

        private:
            struct LightingShaderConst {
                uint32_t maxLights;
                uint32_t maxShadowLights;
                uint32_t numberShadowMaps;
                float shadowMapConstantBias;
                float shadowMapSlopeBiasFactor;
                float maxEmissiveFactor;
            };

            void onCameraProjectionUpdate();

            //model
            void registerModelForAnimation(Model&);
            void unregisterModelForAnimation(Model&);

            //scene
            void createOrUpdateLightingPass();
            void createOrUpdateLightingShader();
            void updateScene(float);
            void deferredRendering(std::uint32_t, float);
            unsigned int computeDependenciesToFirstPassOutput() const;
            void renderDebugSceneData(GeometryContainer&);
            void lightingPassRendering(std::uint32_t);
            unsigned int computeDependenciesToSecondPassOutput() const;
            void renderDebugFramebuffers(unsigned int);
            void postUpdateScene();

            static constexpr uint32_t POSITIONING_DATA_UNIFORM_BINDING = 0;
            static constexpr uint32_t VISUAL_OPTION_UNIFORM_BINDING = 1;
            static constexpr uint32_t LIGHTS_DATA_UNIFORM_BINDING = 2;
            static constexpr uint32_t SM_PROJ_VIEW_MATRICES_UNIFORM_BINDING = 3;
            static constexpr uint32_t SM_SPLIT_DATA_UNIFORM_BINDING = 4;
            static constexpr uint32_t SM_RESOLUTION_UNIFORM_BINDING = 5;
            static constexpr uint32_t FOG_UNIFORM_BINDING = 6;
            static constexpr uint32_t DEPTH_TEX_UNIFORM_BINDING = 7;
            static constexpr uint32_t ALBEDO_TEX_UNIFORM_BINDING = 8;
            static constexpr uint32_t NORMAL_AMBIENT_TEX_UNIFORM_BINDING = 9;
            static constexpr uint32_t MATERIAL_TEX_UNIFORM_BINDING = 10;
            static constexpr uint32_t AO_TEX_UNIFORM_BINDING = 11;
            static constexpr uint32_t SM_TEX_UNIFORM_BINDING = 12;

            //scene properties
            RenderTarget& finalRenderTarget;
            unsigned int sceneWidth;
            unsigned int sceneHeight;
            bool paused;
            std::shared_ptr<Camera> camera;
            float renderingScale;

            //deferred rendering
            std::unique_ptr<RenderTarget> deferredRenderTarget;
            ModelOcclusionCuller modelOcclusionCuller;
            ModelSetDisplayer modelSetDisplayer;
            std::unordered_set<Model*> modelsAnimated;
            std::shared_ptr<AABBoxModel> debugOcclusionCullerGeometries;
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

            std::shared_ptr<Texture> albedoTexture;
            std::shared_ptr<Texture> normalAndAmbientTexture;
            std::shared_ptr<Texture> materialTexture;
            std::shared_ptr<Texture> illuminatedTexture;

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
