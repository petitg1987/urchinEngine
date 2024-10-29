#pragma once

#include <string>
#include <memory>
#include <vector>
#include <UrchinCommon.h>

#include <scene/Renderer.h>
#include <scene/renderer3d/VisualConfig.h>
#include <scene/renderer3d/camera/Camera.h>
#include <scene/renderer3d/postprocess/antialiasing/AntiAliasingApplier.h>
#include <scene/renderer3d/postprocess/reflection/ReflectionApplier.h>
#include <scene/renderer3d/postprocess/bloom/BloomEffectApplier.h>
#include <scene/renderer3d/postprocess/output/OutputRenderer.h>
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
            void activateShadow(bool);
            bool isShadowActivated() const;
            ShadowManager& getShadowManager();
            void activateAmbientOcclusion(bool);
            bool isAmbientOcclusionActivated() const;
            AmbientOcclusionManager& getAmbientOcclusionManager();
            TransparentManager& getTransparentManager();
            void activateAntiAliasing(bool);
            AntiAliasingApplier& getAntiAliasingApplier();
            BloomEffectApplier& getBloomEffectApplier();
            void activateReflection(bool);
            ReflectionApplier& getReflectionApplier();
            void updateRenderingScale(float);
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
            struct DeferredSecondPassShaderConst {
                uint32_t maxLights;
                float ambientOcclusionStrength;
                uint32_t maxShadowLights;
                uint32_t numberShadowMaps;
                float shadowMapConstantBias;
                float shadowMapSlopeBiasFactor;
                uint32_t shadowMapOffsetTexSize;
                float maxEmissiveFactor;
            };

            void onCameraProjectionUpdate();

            //model
            void registerModelForAnimation(Model&);
            void unregisterModelForAnimation(Model&);

            //scene
            void createOrUpdateDeferredPasses();
            void createOrUpdateDeferredSecondPassShader();
            void updateScene(float);
            void renderDeferredFirstPass(std::uint32_t, float);
            unsigned int computeDependenciesToFirstPassOutput() const;
            void renderDebugSceneData(GeometryContainer&);
            void renderDeferredSecondPass(std::uint32_t);
            unsigned int computeDependenciesToSecondPassOutput() const;
            void renderDebugFramebuffers(unsigned int);
            void postUpdateScene();

            static constexpr uint32_t POSITIONING_DATA_UNIFORM_BINDING = 0;
            static constexpr uint32_t SCENE_INFO_UNIFORM_BINDING = 1;
            static constexpr uint32_t LIGHTS_DATA_UNIFORM_BINDING = 2;
            static constexpr uint32_t SM_PROJ_VIEW_MATRICES_UNIFORM_BINDING = 3;
            static constexpr uint32_t SM_DATA_UNIFORM_BINDING = 4;
            static constexpr uint32_t SM_INFO_UNIFORM_BINDING = 5;
            static constexpr uint32_t FOG_UNIFORM_BINDING = 6;
            static constexpr uint32_t DEPTH_TEX_UNIFORM_BINDING = 7;
            static constexpr uint32_t ALBEDO_EMISSIVE_TEX_UNIFORM_BINDING = 8;
            static constexpr uint32_t NORMAL_AMBIENT_TEX_UNIFORM_BINDING = 9;
            static constexpr uint32_t MATERIAL_TEX_UNIFORM_BINDING = 10;
            static constexpr uint32_t AO_TEX_UNIFORM_BINDING = 11;
            static constexpr uint32_t SM_TEX_UNIFORM_BINDING = 12;
            static constexpr uint32_t SM_OFFSET_TEX_UNIFORM_BINDING = 13;

            //scene properties
            RenderTarget& finalRenderTarget;
            unsigned int sceneWidth;
            unsigned int sceneHeight;
            bool paused;
            std::shared_ptr<Camera> camera;
            float renderingScale;

            //deferred rendering
            std::unique_ptr<RenderTarget> deferredFirstPassRenderTarget;
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
            ShadowManager shadowManager;

            std::shared_ptr<Texture> albedoAndEmissiveTexture;
            std::shared_ptr<Texture> normalAndAmbientTexture;
            std::shared_ptr<Texture> materialTexture;
            std::shared_ptr<Texture> illuminatedTexture;

            //deferred second pass rendering + post process
            std::unique_ptr<RenderTarget> deferredSecondPassRenderTarget;
            std::unique_ptr<GenericRenderer> deferredSecondPassRenderer;
            std::unique_ptr<Shader> deferredSecondPassShader;
            struct {
                alignas(16) Matrix4<float> inverseProjectionViewMatrix;
                alignas(16) Point3<float> viewPosition;
            } positioningData;
            struct {
                alignas(8) Point2<float> sceneSize;
                alignas(4) bool isShadowActivated;
                alignas(4) bool isAmbientOcclusionActivated;
            } sceneInfo;

            //post processing
            AntiAliasingApplier antiAliasingApplier;
            bool isAntiAliasingActivated;
            BloomEffectApplier bloomEffectApplier;
            ReflectionApplier reflectionApplier;
            bool isReflectionActivated;
            TransparentManager transparentManager;
            OutputRenderer outputRenderer;

            //debug
            bool refreshDebugFramebuffers;
            std::vector<std::unique_ptr<TextureRenderer>> debugFramebuffers;
    };

}
