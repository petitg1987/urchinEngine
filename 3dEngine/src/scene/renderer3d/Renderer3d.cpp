#include <stdexcept>
#include <locale>

#include <scene/renderer3d/Renderer3d.h>
#include <scene/renderer3d/util/OpaqueMeshFilter.h>
#include <scene/renderer3d/util/OctreeRenderer.h>
#include <graphic/render/GenericRendererBuilder.h>
#include <graphic/render/shader/builder/ShaderBuilder.h>

namespace urchin {

    //debug parameters
    bool DEBUG_DISPLAY_DEPTH_BUFFER = false;
    bool DEBUG_DISPLAY_COLOR_BUFFER = false;
    bool DEBUG_DISPLAY_NORMAL_AMBIENT_BUFFER = false;
    bool DEBUG_DISPLAY_ILLUMINATED_SCENE_BUFFER = false;
    bool DEBUG_DISPLAY_SHADOW_MAP_BUFFER = false;
    bool DEBUG_DISPLAY_AMBIENT_OCCLUSION_BUFFER = false;
    bool DEBUG_DISPLAY_MODELS_OCTREE = false;
    bool DEBUG_DISPLAY_MODELS_BOUNDING_BOX = false;
    bool DEBUG_DISPLAY_MODEL_BASE_BONES = false;
    bool DEBUG_DISPLAY_LIGHTS_OCTREE = false;

    //static
    constexpr float Renderer3d::MODELS_OCTREE_MIN_SIZE = 20.0f;

    Renderer3d::Renderer3d(RenderTarget& finalRenderTarget) :
            //scene properties
            finalRenderTarget(finalRenderTarget),
            sceneWidth(finalRenderTarget.getWidth()),
            sceneHeight(finalRenderTarget.getHeight()),
            paused(true),

            //deferred rendering
            deferredRenderTarget(std::make_unique<OffscreenRender>("deferred rendering - first pass", RenderTarget::SHARED_DEPTH_ATTACHMENT)),
            modelOctreeManager(std::make_unique<OctreeManager<Model>>(MODELS_OCTREE_MIN_SIZE)),
            modelSetDisplayer(std::make_unique<ModelSetDisplayer>(DisplayMode::DEFAULT_MODE)),
            fogContainer(std::make_unique<FogContainer>()),
            terrainContainer(std::make_unique<TerrainContainer>(*deferredRenderTarget)),
            waterContainer(std::make_unique<WaterContainer>(*deferredRenderTarget)),
            geometryContainer(std::make_unique<GeometryContainer>(*deferredRenderTarget)),
            skyContainer(std::make_unique<SkyContainer>(*deferredRenderTarget)),
            lightManager(std::make_unique<LightManager>(*deferredRenderTarget)),
            ambientOcclusionManager(std::make_unique<AmbientOcclusionManager>()),
            transparentManager(std::make_unique<TransparentManager>(*lightManager)),
            shadowManager(std::make_unique<ShadowManager>(*lightManager, *modelOctreeManager)),

            //lighting pass rendering
            offscreenLightingRenderTarget(std::make_unique<OffscreenRender>("deferred rendering - second pass", RenderTarget::NO_DEPTH_ATTACHMENT)),
            positioningData({}),
            visualOption({}),
            antiAliasingManager(std::make_unique<AntiAliasingManager>(finalRenderTarget)),
            isAntiAliasingActivated(true),

            //debug
            refreshDebugFramebuffers(true) {

        //deferred rendering
        modelSetDisplayer->setupMeshFilter(std::make_unique<OpaqueMeshFilter>());
        modelSetDisplayer->initialize(*deferredRenderTarget);
        shadowManager->addObserver(this, ShadowManager::NUMBER_SHADOW_MAPS_UPDATE);

        //lighting pass rendering
        visualOption.isShadowActivated = true;
        visualOption.isAmbientOcclusionActivated = true;
        createOrUpdateLightingPass();
    }

    Renderer3d::~Renderer3d() {
        offscreenLightingRenderTarget->cleanup();
        deferredRenderTarget->cleanup();
    }

    void Renderer3d::onResize(unsigned int sceneWidth, unsigned int sceneHeight) {
        //scene properties
        this->sceneWidth = sceneWidth;
        this->sceneHeight = sceneHeight;

        //camera
        if (camera) {
            camera->onResize(sceneWidth, sceneHeight);
            onCameraProjectionUpdate();
        }

        createOrUpdateLightingPass();
    }

    void Renderer3d::notify(Observable* observable, int notificationType) {
        if (dynamic_cast<ShadowManager*>(observable)) {
            if (notificationType == ShadowManager::NUMBER_SHADOW_MAPS_UPDATE) {
                createOrUpdateLightingPass();
            }
        }
    }

    OctreeManager<Model>& Renderer3d::getModelOctreeManager() const {
        return *modelOctreeManager;
    }

    FogContainer& Renderer3d::getFogContainer() const {
        return *fogContainer;
    }

    TerrainContainer& Renderer3d::getTerrainContainer() const {
        return *terrainContainer;
    }

    WaterContainer& Renderer3d::getWaterContainer() const {
        return *waterContainer;
    }

    GeometryContainer& Renderer3d::getGeometryContainer() const {
        return *geometryContainer;
    }

    SkyContainer& Renderer3d::getSkyContainer() const {
        return *skyContainer;
    }

    LightManager& Renderer3d::getLightManager() const {
        return *lightManager;
    }

    ShadowManager& Renderer3d::getShadowManager() const {
        return *shadowManager;
    }

    void Renderer3d::activateShadow(bool isShadowActivated) {
        if (visualOption.isShadowActivated != isShadowActivated) {
            visualOption.isShadowActivated = isShadowActivated;

            createOrUpdateLightingPass();
            shadowManager->forceUpdateAllShadowMaps();
        }
    }

    AmbientOcclusionManager& Renderer3d::getAmbientOcclusionManager() const {
        return *ambientOcclusionManager;
    }

    void Renderer3d::activateAmbientOcclusion(bool isAmbientOcclusionActivated) {
        if (visualOption.isAmbientOcclusionActivated != isAmbientOcclusionActivated) {
            visualOption.isAmbientOcclusionActivated = isAmbientOcclusionActivated;
            createOrUpdateLightingPass();
        }
    }

    TransparentManager& Renderer3d::getTransparentManager() const {
        return *transparentManager;
    }

    AntiAliasingManager& Renderer3d::getAntiAliasingManager() const {
        return *antiAliasingManager;
    }

    void Renderer3d::activateAntiAliasing(bool isAntiAliasingActivated) {
        if (this->isAntiAliasingActivated != isAntiAliasingActivated) {
            this->isAntiAliasingActivated = isAntiAliasingActivated;
            createOrUpdateLightingPass();
        }
    }

    void Renderer3d::setCamera(std::shared_ptr<Camera> camera) {
        if (this->camera != nullptr) {
           throw std::runtime_error("Redefine a camera is currently not supported");
        }

        this->camera = std::move(camera);
        if (this->camera) {
            this->camera->initialize(sceneWidth, sceneHeight);
            onCameraProjectionUpdate();
        }
    }

    void Renderer3d::onCameraProjectionUpdate() {
        modelSetDisplayer->onCameraProjectionUpdate(*camera);
        terrainContainer->onCameraProjectionUpdate(*camera);
        waterContainer->onCameraProjectionUpdate(*camera);
        geometryContainer->onCameraProjectionUpdate(*camera);
        skyContainer->onCameraProjectionUpdate(*camera);
        shadowManager->onCameraProjectionUpdate(*camera);
        ambientOcclusionManager->onCameraProjectionUpdate(*camera);
        transparentManager->onCameraProjectionUpdate(*camera);
        refreshDebugFramebuffers = true;
    }

    void Renderer3d::updateModelsInFrustum() {
        modelsInFrustum.clear();
        modelOctreeManager->getOctreeablesIn(getCamera()->getFrustum(), modelsInFrustum);
    }

    Camera* Renderer3d::getCamera() const {
        return camera.get();
    }

    void Renderer3d::addModel(std::shared_ptr<Model> model) {
        if (model) {
            Logger::instance().logInfo("Add 3D model: " + model->getConstMeshes()->getName());
            modelOctreeManager->addOctreeable(std::move(model));
        }
    }

    std::shared_ptr<Model> Renderer3d::removeModel(Model* model) {
        if (model) {
            shadowManager->removeModel(model);
            transparentManager->removeModel(model);
            modelSetDisplayer->removeModel(model);
            return modelOctreeManager->removeOctreeable(model);
        }
        return std::shared_ptr<Model>(nullptr);
    }

    void Renderer3d::pause() {
        paused = true;
    }

    void Renderer3d::unpause() {
        if (camera) {
            camera->resetPreviousMousePosition();
        }

        paused = false;
    }

    bool Renderer3d::isPaused() const {
        return paused;
    }

    bool Renderer3d::onKeyPress(unsigned int key) {
        if (!paused && camera && key < 260) {
            return camera->onKeyPress(key);
        }
        return true;
    }

    bool Renderer3d::onKeyRelease(unsigned int key) {
        if (!paused && camera && key < 260) {
            return camera->onKeyRelease(key);
        }
        return true;
    }

    bool Renderer3d::onChar(char32_t) {
        //nothing to do
        return true;
    }

    bool Renderer3d::onMouseMove(double mouseX, double mouseY) {
        if (!paused && camera) {
            return camera->onMouseMove(mouseX, mouseY);
        }
        return true;
    }

    bool Renderer3d::onScroll(double) {
        //nothing to do
        return true;
    }

    void Renderer3d::onDisable() {
        //nothing to do
    }

    void Renderer3d::prepareRendering(float dt) {
        ScopeProfiler sp(Profiler::graphic(), "pre3dRendering");

        if (!camera) { //nothing to display if camera doesn't exist
            return;
        }

        updateScene(dt);
        deferredRendering(dt);
        lightingPassRendering();
        if (isAntiAliasingActivated) {
            antiAliasingManager->applyAntiAliasing();
        }

        renderDebugFramebuffers();
        postUpdateScene();
    }

    void Renderer3d::createOrUpdateLightingPass() {
        //deferred rendering
        diffuseTexture = Texture::build(sceneWidth, sceneHeight, TextureFormat::RGBA_8_INT, nullptr);
        normalAndAmbientTexture = Texture::build(sceneWidth, sceneHeight, TextureFormat::RGBA_8_INT, nullptr);
        deferredRenderTarget->resetTextures();
        deferredRenderTarget->addTexture(diffuseTexture);
        deferredRenderTarget->addTexture(normalAndAmbientTexture);
        deferredRenderTarget->initialize();

        //lighting pass rendering
        RenderTarget* lightingRenderTarget = &finalRenderTarget;
        if (isAntiAliasingActivated) {
            lightingPassTexture = Texture::build(sceneWidth, sceneHeight, TextureFormat::RGBA_8_INT, nullptr);
            offscreenLightingRenderTarget->resetTextures();
            offscreenLightingRenderTarget->addTexture(lightingPassTexture);
            offscreenLightingRenderTarget->initialize();

            lightingRenderTarget = offscreenLightingRenderTarget.get();
        }

        createOrUpdateLightingShader();

        std::vector<Point2<float>> vertexCoord = {
                Point2<float>(-1.0f, -1.0f), Point2<float>(1.0f, -1.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(-1.0f, -1.0f), Point2<float>(1.0f, 1.0f), Point2<float>(-1.0f, 1.0f)
        };
        std::vector<Point2<float>> textureCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 0.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(0.0f, 1.0f)
        };

        auto lightingRendererBuilder = GenericRendererBuilder::create("deferred rendering - second pass", *lightingRenderTarget, *lightingShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformData(sizeof(positioningData), &positioningData) //binding 0
                ->addUniformData(sizeof(visualOption), &visualOption); //binding 1
        lightManager->setupLightingRenderer(lightingRendererBuilder); //binding 2
        shadowManager->setupLightingRenderer(lightingRendererBuilder); //binding 3 & 4
        fogContainer->setupLightingRenderer(lightingRendererBuilder); //binding 5

        std::vector<std::shared_ptr<TextureReader>> shadowMapTextureReaders;
        for (unsigned int i = 0; i < shadowManager->getMaxShadowLights(); ++i) {
            shadowMapTextureReaders.push_back(TextureReader::build(shadowManager->getEmptyShadowMapTexture(), TextureParam::buildNearest()));
        }
        lightingRenderer = lightingRendererBuilder
                ->addUniformTextureReader(TextureReader::build(deferredRenderTarget->getDepthTexture(), TextureParam::buildNearest())) //binding 6
                ->addUniformTextureReader(TextureReader::build(diffuseTexture, TextureParam::buildNearest())) //binding 7
                ->addUniformTextureReader(TextureReader::build(normalAndAmbientTexture, TextureParam::buildNearest())) //binding 8
                ->addUniformTextureReader(TextureReader::build(Texture::buildEmptyGreyscale8Int(), TextureParam::buildNearest())) //binding 9 - ambient occlusion
                ->addUniformTextureReader(TextureReader::build(Texture::buildEmptyRgba16Float(), TextureParam::buildNearest())) //binding 10 - transparency: accumulation
                ->addUniformTextureReader(TextureReader::build(Texture::buildEmptyGreyscale8Int(), TextureParam::buildNearest())) //binding 11 - transparency: reveal
                ->addUniformTextureReaderArray(shadowMapTextureReaders) //binding 12
                ->build();

        ambientOcclusionManager->onSizeUpdate(deferredRenderTarget->getDepthTexture(), normalAndAmbientTexture);
        transparentManager->onSizeUpdate(deferredRenderTarget->getDepthTexture());

        if (isAntiAliasingActivated) {
            antiAliasingManager->onSizeUpdate(lightingPassTexture);
        }

        refreshDebugFramebuffers = true;
    }

    void Renderer3d::createOrUpdateLightingShader() {
        LightingShaderConst lightingConstData{};
        lightingConstData.maxLights = lightManager->getMaxLights();
        lightingConstData.maxShadowLights = shadowManager->getMaxShadowLights();
        lightingConstData.numberShadowMaps = shadowManager->getConfig().nbShadowMaps;
        lightingConstData.shadowMapBias = shadowManager->getShadowMapBias();
        std::vector<std::size_t> variablesSize = {
                sizeof(LightingShaderConst::maxLights),
                sizeof(LightingShaderConst::maxShadowLights),
                sizeof(LightingShaderConst::numberShadowMaps),
                sizeof(LightingShaderConst::shadowMapBias)
        };
        auto shaderConstants = std::make_unique<ShaderConstants>(variablesSize, &lightingConstData);

        lightingShader = ShaderBuilder::createShader("lighting.vert.spv", "", "lighting.frag.spv", std::move(shaderConstants));
    }

    void Renderer3d::updateScene(float dt) {
        ScopeProfiler sp(Profiler::graphic(), "updateScene");

        //move the camera
        camera->updateCameraView(dt);

        //refresh models in octree
        modelOctreeManager->refreshOctreeables();

        //determine model visible on scene
        updateModelsInFrustum();
        modelSetDisplayer->setModels(modelsInFrustum);

        //determine visible lights on scene
        lightManager->updateVisibleLights(camera->getFrustum());

        if (visualOption.isShadowActivated) {
            //determine models producing shadow on scene
            shadowManager->updateVisibleModels(camera->getFrustum());

            //animate models
            for (auto model : shadowManager->getVisibleModels()) {
                model->updateAnimation(dt);
            }
        } else {
            //animate models
            for (auto model : modelsInFrustum) {
                model->updateAnimation(dt);
            }
        }

        transparentManager->updateModels(modelsInFrustum);
    }

    /**
     * First pass of deferred shading algorithm.
     * Render depth, color, normal, etc. into buffers.
     */
    void Renderer3d::deferredRendering(float dt) {
        ScopeProfiler sp(Profiler::graphic(), "deferredRender");

        //deferred shadow map
        if (visualOption.isShadowActivated) {
            shadowManager->updateShadowMaps();
        }

        //deferred scene (depth, color, normal, ambient)
        deferredRenderTarget->disableAllRenderers();
        skyContainer->prepareRendering(camera->getViewMatrix(), camera->getPosition());
        modelSetDisplayer->prepareRendering(camera->getViewMatrix());
        terrainContainer->prepareRendering(*camera, dt);
        waterContainer->prepareRendering(*camera, fogContainer.get(), dt);
        geometryContainer->prepareRendering(camera->getViewMatrix());
        renderDebugSceneData();
        deferredRenderTarget->render();

        //deferred ambient occlusion
        if (visualOption.isAmbientOcclusionActivated) {
            ambientOcclusionManager->updateAOTexture(*camera);
        }

        transparentManager->updateTransparentTextures(*camera);
    }

    void Renderer3d::renderDebugSceneData() {
        if (DEBUG_DISPLAY_MODELS_OCTREE) {
            debugModelOctree = OctreeRenderer::createOctreeModel(*modelOctreeManager, *deferredRenderTarget, camera->getProjectionMatrix());
            debugModelOctree->prepareRendering(camera->getViewMatrix());
        }

        if (DEBUG_DISPLAY_MODELS_BOUNDING_BOX) {
            modelSetDisplayer->drawBBox(camera->getProjectionMatrix(), camera->getViewMatrix());
        }

        if (DEBUG_DISPLAY_MODEL_BASE_BONES) {
            modelSetDisplayer->drawBaseBones(camera->getProjectionMatrix(), camera->getViewMatrix(), "models/characterAnimate.urchinMesh");
        }

        if (DEBUG_DISPLAY_LIGHTS_OCTREE) {
            lightManager->drawLightOctree(camera->getProjectionMatrix(), camera->getViewMatrix());
        }
    }

    /**
     * Second pass of deferred shading algorithm.
     * Compute lighting in pixel shader and render the scene to screen.
     */
    void Renderer3d::lightingPassRendering() {
        ScopeProfiler sp(Profiler::graphic(), "lightPassRender");

        positioningData.inverseProjectionViewMatrix = (camera->getProjectionMatrix() * camera->getViewMatrix()).inverse();
        positioningData.viewPosition = camera->getPosition();
        constexpr std::size_t positioningDataUniformIndex = 0;
        lightingRenderer->updateUniformData(positioningDataUniformIndex, &positioningData);

        constexpr std::size_t lightsDataUniformIndex = 2;
        lightManager->loadVisibleLights(*lightingRenderer, lightsDataUniformIndex);

        constexpr std::size_t fogUniformIndex = 5;
        fogContainer->loadFog(*lightingRenderer, fogUniformIndex);

        if (visualOption.isAmbientOcclusionActivated) {
            constexpr std::size_t ambientOcclusionTexUnit = 3;
            ambientOcclusionManager->loadAOTexture(*lightingRenderer, ambientOcclusionTexUnit);
        }

        constexpr std::size_t transparentAccumulationTexUnit = 4;
        constexpr std::size_t transparentRevealTexUnit = 5;
        transparentManager->loadTransparentTextures(*lightingRenderer, transparentAccumulationTexUnit, transparentRevealTexUnit);

        if (visualOption.isShadowActivated) {
            constexpr std::size_t shadowMapTexUnit = 6;
            shadowManager->loadShadowMaps(*lightingRenderer, shadowMapTexUnit);
        }

        if (isAntiAliasingActivated) {
            offscreenLightingRenderTarget->render();
        } else {
            lightingRenderer->enableRenderer();
        }
    }

    void Renderer3d::renderDebugFramebuffers() {
        if(refreshDebugFramebuffers) {
            debugFramebuffers.clear();
            refreshDebugFramebuffers = false;

            if (DEBUG_DISPLAY_DEPTH_BUFFER) {
                auto textureRenderer = std::make_unique<TextureRenderer>(deferredRenderTarget->getDepthTexture(), TextureRenderer::GRAYSCALE_VALUE);
                textureRenderer->setPosition(TextureRenderer::LEFT, TextureRenderer::TOP);
                textureRenderer->initialize("[DEBUG] depth texture", finalRenderTarget, sceneWidth, sceneHeight, 0.95f, 1.0f);
                debugFramebuffers.emplace_back(std::move(textureRenderer));
            }

            if (DEBUG_DISPLAY_COLOR_BUFFER) {
                auto textureRenderer = std::make_unique<TextureRenderer>(diffuseTexture, TextureRenderer::DEFAULT_VALUE);
                textureRenderer->setPosition(TextureRenderer::CENTER_X, TextureRenderer::TOP);
                textureRenderer->initialize("[DEBUG] diffuse texture", finalRenderTarget, sceneWidth, sceneHeight);
                debugFramebuffers.emplace_back(std::move(textureRenderer));
            }

            if (DEBUG_DISPLAY_NORMAL_AMBIENT_BUFFER) {
                auto textureRenderer = std::make_unique<TextureRenderer>(normalAndAmbientTexture, TextureRenderer::DEFAULT_VALUE);
                textureRenderer->setPosition(TextureRenderer::RIGHT, TextureRenderer::TOP);
                textureRenderer->initialize("[DEBUG] normal/ambient texture", finalRenderTarget, sceneWidth, sceneHeight);
                debugFramebuffers.emplace_back(std::move(textureRenderer));
            }

            if (DEBUG_DISPLAY_ILLUMINATED_SCENE_BUFFER) {
                auto textureRenderer = std::make_unique<TextureRenderer>(lightingPassTexture, TextureRenderer::DEFAULT_VALUE);
                textureRenderer->setPosition(TextureRenderer::LEFT, TextureRenderer::BOTTOM);
                textureRenderer->initialize("[DEBUG] lighting pass texture", finalRenderTarget, sceneWidth, sceneHeight);
                debugFramebuffers.emplace_back(std::move(textureRenderer));
            }

            if (DEBUG_DISPLAY_SHADOW_MAP_BUFFER && visualOption.isShadowActivated && !lightManager->getSunLights().empty()) {
                const std::shared_ptr<Light> firstLight = lightManager->getSunLights()[0]; //choose light
                const unsigned int shadowMapNumber = 0; //choose shadow map to display [0, nbShadowMaps - 1]
                auto textureRenderer = std::make_unique<TextureRenderer>(shadowManager->getLightShadowMap(firstLight.get()).getShadowMapTexture(), shadowMapNumber, TextureRenderer::DEFAULT_VALUE);
                textureRenderer->setPosition(TextureRenderer::CENTER_X, TextureRenderer::BOTTOM);
                textureRenderer->initialize("[DEBUG] shadow map", finalRenderTarget, sceneWidth, sceneHeight);
                debugFramebuffers.emplace_back(std::move(textureRenderer));
            }

            if (DEBUG_DISPLAY_AMBIENT_OCCLUSION_BUFFER && visualOption.isAmbientOcclusionActivated) {
                auto textureRenderer = std::make_unique<TextureRenderer>(ambientOcclusionManager->getAmbientOcclusionTexture(), TextureRenderer::INVERSE_GRAYSCALE_VALUE);
                textureRenderer->setPosition(TextureRenderer::CENTER_X, TextureRenderer::BOTTOM);
                textureRenderer->initialize("[DEBUG] ambient occlusion texture", finalRenderTarget, sceneWidth, sceneHeight, 0.0f, 0.05f);
                debugFramebuffers.emplace_back(std::move(textureRenderer));
            }
        } else {
            for (auto& debugFramebuffer : debugFramebuffers) {
                debugFramebuffer->prepareRendering();
            }
        }
    }

    void Renderer3d::postUpdateScene() {
        ScopeProfiler sp(Profiler::graphic(), "postUpdateScene");

        modelOctreeManager->postRefreshOctreeables();
        lightManager->postUpdateVisibleLights();
    }

}
