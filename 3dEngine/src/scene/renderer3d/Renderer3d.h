#pragma once

#include <string>
#include <memory>
#include <vector>
#include <UrchinCommon.h>

#include <scene/Renderer.h>
#include <scene/renderer3d/camera/Camera.h>
#include <scene/renderer3d/model/displayer/ModelSetDisplayer.h>
#include <scene/renderer3d/postprocess/antialiasing/AntiAliasingManager.h>
#include <scene/renderer3d/postprocess/ambientocclusion/AmbientOcclusionManager.h>
#include <scene/renderer3d/transparent/TransparentManager.h>
#include <scene/renderer3d/lighting/shadow/ShadowManager.h>
#include <scene/renderer3d/model/Model.h>
#include <scene/renderer3d/model/displayer/ModelSetDisplayer.h>
#include <scene/renderer3d/landscape/terrain/TerrainContainer.h>
#include <scene/renderer3d/landscape/fog/FogContainer.h>
#include <scene/renderer3d/landscape/water/WaterContainer.h>
#include <scene/renderer3d/landscape/sky/SkyContainer.h>
#include <resources/geometry/GeometryContainer.h>
#include <resources/geometry/GeometryModel.h>
#include <graphic/render/shader/model/Shader.h>
#include <graphic/render/GenericRenderer.h>
#include <graphic/render/target/ScreenRender.h>
#include <graphic/render/target/OffscreenRender.h>
#include <graphic/texture/Texture.h>
#include <texture/render/TextureRenderer.h>

namespace urchin {

    class Renderer3d : public Renderer, public Observer {
        public:
            explicit Renderer3d(RenderTarget&);
            ~Renderer3d() override;

            //scene properties
            void onResize(unsigned int, unsigned int) override;
            void notify(Observable*, int) override;

            //graphics
            OctreeManager<Model>& getModelOctreeManager() const;
            FogContainer& getFogContainer() const;
            TerrainContainer& getTerrainContainer() const;
            WaterContainer& getWaterContainer() const;
            GeometryContainer& getGeometryContainer() const;
            SkyContainer& getSkyContainer() const;
            LightManager& getLightManager() const;
            ShadowManager& getShadowManager() const;
            void activateShadow(bool);
            AmbientOcclusionManager& getAmbientOcclusionManager() const;
            void activateAmbientOcclusion(bool);
            TransparentManager& getTransparentManager() const;
            AntiAliasingManager& getAntiAliasingManager() const;
            void activateAntiAliasing(bool);

            //camera
            void setCamera(std::shared_ptr<Camera>);
            Camera* getCamera() const;

            //models
            void addModel(std::shared_ptr<Model>);
            std::shared_ptr<Model> removeModel(Model*);

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
            void prepareRendering(float) override;

        private:
            struct LightingShaderConst {
                uint32_t maxLights;
                uint32_t maxShadowLights;
                uint32_t numberShadowMaps;
                float shadowMapBias;
            };

            void onCameraProjectionUpdate();

            //model
            void updateModelsInFrustum();

            //scene
            void createOrUpdateLightingPass();
            void createOrUpdateLightingShader();
            void updateScene(float);
            void deferredRendering(float);
            void renderDebugSceneData();
            void lightingPassRendering();
            void renderDebugFramebuffers();
            void postUpdateScene();

            static const float MODELS_OCTREE_MIN_SIZE;

            //scene properties
            RenderTarget& finalRenderTarget;
            unsigned int sceneWidth, sceneHeight;
            bool paused;
            std::shared_ptr<Camera> camera;

            //deferred rendering
            std::unique_ptr<OffscreenRender> deferredRenderTarget;
            std::unique_ptr<OctreeManager<Model>> modelOctreeManager;
            std::unique_ptr<ModelSetDisplayer> modelSetDisplayer;
            std::unique_ptr<AABBoxModel> debugModelOctree;
            std::vector<Model*> modelsInFrustum;

            std::unique_ptr<FogContainer> fogContainer;
            std::unique_ptr<TerrainContainer> terrainContainer;
            std::unique_ptr<WaterContainer> waterContainer;
            std::unique_ptr<GeometryContainer> geometryContainer;
            std::unique_ptr<SkyContainer> skyContainer;
            std::unique_ptr<LightManager> lightManager;
            std::unique_ptr<AmbientOcclusionManager> ambientOcclusionManager;
            std::unique_ptr<TransparentManager> transparentManager;
            std::unique_ptr<ShadowManager> shadowManager;

            std::shared_ptr<Texture> diffuseTexture, normalAndAmbientTexture, lightingPassTexture;

            //lighting pass rendering
            std::unique_ptr<OffscreenRender> offscreenLightingRenderTarget;
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
            std::unique_ptr<AntiAliasingManager> antiAliasingManager;
            bool isAntiAliasingActivated;

            //debug
            bool refreshDebugFramebuffers;
            std::vector<std::unique_ptr<TextureRenderer>> debugFramebuffers;
    };

}
