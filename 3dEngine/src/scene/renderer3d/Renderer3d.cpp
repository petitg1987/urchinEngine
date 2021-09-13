#include <stdexcept>
#include <locale>

#include <scene/renderer3d/Renderer3d.h>
#include <scene/renderer3d/util/OpaqueMeshFilter.h>
#include <scene/renderer3d/util/OctreeRenderer.h>
#include <graphic/render/GenericRendererBuilder.h>
#include <graphic/render/shader/builder/ShaderBuilder.h>
#include <graphic/render/pipeline/PipelineContainer.h>
#include <graphic/render/target/NullRenderTarget.h>

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

    Renderer3d::Renderer3d(RenderTarget& finalRenderTarget) :
            //scene properties
            finalRenderTarget(finalRenderTarget),
            sceneWidth(finalRenderTarget.getWidth()),
            sceneHeight(finalRenderTarget.getHeight()),
            paused(true),

            //deferred rendering
            deferredRenderTarget(finalRenderTarget.isValidRenderTarget() ?
                    std::unique_ptr<RenderTarget>(new OffscreenRender("deferred rendering - first pass", RenderTarget::SHARED_DEPTH_ATTACHMENT)) :
                    std::unique_ptr<RenderTarget>(new NullRenderTarget(finalRenderTarget.getWidth(), finalRenderTarget.getHeight()))),
            modelOctreeManager(std::make_unique<OctreeManager<Model>>(MODELS_OCTREE_MIN_SIZE)),
            modelSetDisplayer(std::make_unique<ModelSetDisplayer>(DisplayMode::DEFAULT_MODE)),
            fogContainer(std::make_unique<FogContainer>()),
            terrainContainer(std::make_unique<TerrainContainer>(*deferredRenderTarget)),
            waterContainer(std::make_unique<WaterContainer>(*deferredRenderTarget)),
            geometryContainer(std::make_unique<GeometryContainer>(*deferredRenderTarget)),
            skyContainer(std::make_unique<SkyContainer>(*deferredRenderTarget)),
            lightManager(std::make_unique<LightManager>()),
            ambientOcclusionManager(std::make_unique<AmbientOcclusionManager>(!finalRenderTarget.isValidRenderTarget())),
            transparentManager(std::make_unique<TransparentManager>(!finalRenderTarget.isValidRenderTarget(), *lightManager)),
            shadowManager(std::make_unique<ShadowManager>(*lightManager, *modelOctreeManager)),

            //lighting pass rendering
            lightingRenderTarget(finalRenderTarget.isValidRenderTarget() ?
                    std::unique_ptr<RenderTarget>(new OffscreenRender("deferred rendering - second pass", RenderTarget::NO_DEPTH_ATTACHMENT)) :
                    std::unique_ptr<RenderTarget>(new NullRenderTarget(finalRenderTarget.getWidth(), finalRenderTarget.getHeight()))),
            positioningData({}),
            visualOption({}),
            antiAliasingApplier(std::make_unique<AntiAliasingApplier>(!finalRenderTarget.isValidRenderTarget())),
            isAntiAliasingActivated(true),
            bloomEffectApplier(std::make_unique<BloomEffectApplier>(finalRenderTarget)),

            //debug
            refreshDebugFramebuffers(true) {
        ScopeProfiler sp(Profiler::graphic(), "render3dInit");

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
        lightingRenderTarget->cleanup();
        deferredRenderTarget->cleanup();
    }

    void Renderer3d::onResize(unsigned int sceneWidth, unsigned int sceneHeight) {
        //scene properties
        this->sceneWidth = sceneWidth;
        this->sceneHeight = sceneHeight;

        //camera
        if (camera) {
            camera->onResize(sceneWidth, sceneHeight);
        }

        createOrUpdateLightingPass();
    }

    void Renderer3d::notify(Observable* observable, int notificationType) {
        if (dynamic_cast<ShadowManager*>(observable)) {
            if (notificationType == ShadowManager::NUMBER_SHADOW_MAPS_UPDATE) {
                createOrUpdateLightingPass();
            }
        } else if (dynamic_cast<Camera*>(observable)) {
            if (notificationType == Camera::PROJECTION_UPDATE) {
                onCameraProjectionUpdate();
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

    AntiAliasingApplier& Renderer3d::getAntiAliasingApplier() const {
        return *antiAliasingApplier;
    }

    BloomEffectApplier& Renderer3d::getBloomEffectApplier() const {
        return *bloomEffectApplier;
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
            this->camera->addObserver(this, Camera::PROJECTION_UPDATE);
            this->camera->initialize(sceneWidth, sceneHeight);
        }
    }

    void Renderer3d::onCameraProjectionUpdate() {
        ScopeProfiler sp(Profiler::graphic(), "render3dProjUp");

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
            ScopeProfiler sp(Profiler::graphic(), "addModel");

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

    /**
     * Pre-warm the models. Pre-warming avoid low frame rate on the first frame.
     * This method must be called once all the models and the camera have been setup.
     */
    void Renderer3d::preWarmModels() {
        if (!camera) {
            throw std::runtime_error("Camera is required to pre warm the models");
        }
        updateScene(0.0f);
        postUpdateScene();
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

    void Renderer3d::pause() {
        paused = true;
    }

    void Renderer3d::unpause() {
        if (camera) {
            camera->resetPreviousMousePosition();
        }

        paused = false;
    }

    void Renderer3d::prepareRendering(float dt, unsigned int& screenRenderingOrder) {
        ScopeProfiler sp(Profiler::graphic(), "pre3dRendering");

        if (!camera) { //nothing to display if camera doesn't exist
            return;
        }

        updateScene(dt);
        deferredRendering(dt);
        lightingPassRendering();
        if (isAntiAliasingActivated) {
            antiAliasingApplier->applyAntiAliasing();
        }
        bloomEffectApplier->applyBloom(screenRenderingOrder);

        screenRenderingOrder++;
        renderDebugFramebuffers(screenRenderingOrder);

        postUpdateScene();
        PipelineContainer::instance().cleanPipelines();
    }

    void Renderer3d::createOrUpdateLightingPass() {
        //deferred rendering
        diffuseTexture = Texture::build(sceneWidth, sceneHeight, TextureFormat::RGBA_8_INT, nullptr);
        normalAndAmbientTexture = Texture::build(sceneWidth, sceneHeight, TextureFormat::RGBA_8_INT, nullptr);
        if (auto* deferredOffscreenRenderTarget = dynamic_cast<OffscreenRender*>(deferredRenderTarget.get())) {
            deferredOffscreenRenderTarget->resetOutputTextures();
            deferredOffscreenRenderTarget->addOutputTexture(diffuseTexture);
            deferredOffscreenRenderTarget->addOutputTexture(normalAndAmbientTexture);
            deferredOffscreenRenderTarget->initialize();
        }

        //lighting pass rendering
        lightingPassTexture = Texture::build(sceneWidth, sceneHeight, TextureFormat::B10G11R11_FLOAT, nullptr);
        if (auto* offscreenLightingRenderTarget = dynamic_cast<OffscreenRender*>(lightingRenderTarget.get())) {
            offscreenLightingRenderTarget->resetOutputTextures();
            offscreenLightingRenderTarget->addOutputTexture(lightingPassTexture);
            offscreenLightingRenderTarget->initialize();
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
                ->addUniformTextureReader(TextureReader::build(Texture::buildEmptyGreyscale(), TextureParam::buildNearest())) //binding 9 - ambient occlusion
                ->addUniformTextureReader(TextureReader::build(Texture::buildEmptyRgba(), TextureParam::buildNearest())) //binding 10 - transparency: accumulation
                ->addUniformTextureReader(TextureReader::build(Texture::buildEmptyGreyscale(), TextureParam::buildNearest())) //binding 11 - transparency: reveal
                ->addUniformTextureReaderArray(shadowMapTextureReaders) //binding 12
                ->build();
        ambientOcclusionManager->onTextureUpdate(deferredRenderTarget->getDepthTexture(), normalAndAmbientTexture);
        transparentManager->onTextureUpdate(deferredRenderTarget->getDepthTexture());

        //post process rendering
        if (isAntiAliasingActivated) {
            antiAliasingApplier->onTextureUpdate(lightingPassTexture);
        }
        bloomEffectApplier->onTextureUpdate(isAntiAliasingActivated ? antiAliasingApplier->getOutputTexture() : lightingPassTexture);

        refreshDebugFramebuffers = true;
    }

    void Renderer3d::createOrUpdateLightingShader() {
        LightingShaderConst lightingConstData{};
        lightingConstData.maxLights = lightManager->getMaxLights();
        lightingConstData.maxShadowLights = shadowManager->getMaxShadowLights();
        lightingConstData.numberShadowMaps = shadowManager->getConfig().nbShadowMaps;
        lightingConstData.shadowMapBias = shadowManager->getShadowMapBias();
        lightingConstData.maxEmissiveFactor = Material::MAX_EMISSIVE_FACTOR;
        std::vector<std::size_t> variablesSize = {
                sizeof(LightingShaderConst::maxLights),
                sizeof(LightingShaderConst::maxShadowLights),
                sizeof(LightingShaderConst::numberShadowMaps),
                sizeof(LightingShaderConst::shadowMapBias),
                sizeof(LightingShaderConst::maxEmissiveFactor)
        };
        auto shaderConstants = std::make_unique<ShaderConstants>(variablesSize, &lightingConstData);

        if (lightingRenderTarget->isValidRenderTarget()) {
            lightingShader = ShaderBuilder::createShader("lighting.vert.spv", "", "lighting.frag.spv", std::move(shaderConstants));
        } else {
            lightingShader = ShaderBuilder::createNullShader();
        }
    }

    void Renderer3d::updateScene(float dt) {
        ScopeProfiler sp(Profiler::graphic(), "updateScene");

        //move the camera
        camera->updateCameraView(dt);

        //refresh models in octree
        modelOctreeManager->refreshOctreeables();

        //determine model visible on scene
        updateModelsInFrustum();
        modelSetDisplayer->updateModels(modelsInFrustum);

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

        //deferred scene (depth, color, normal, ambient...)
        deferredRenderTarget->disableAllRenderers();

        unsigned int deferredRenderingOrder = 0;
        skyContainer->prepareRendering(deferredRenderingOrder, camera->getViewMatrix(), camera->getPosition());

        deferredRenderingOrder++;
        modelSetDisplayer->prepareRendering(deferredRenderingOrder, camera->getViewMatrix());

        deferredRenderingOrder++;
        terrainContainer->prepareRendering(deferredRenderingOrder, *camera, dt);

        deferredRenderingOrder++;
        waterContainer->prepareRendering(deferredRenderingOrder, *camera, fogContainer.get(), dt);

        renderDebugSceneData(*geometryContainer);

        deferredRenderingOrder++;
        geometryContainer->prepareRendering(deferredRenderingOrder, camera->getViewMatrix());

        deferredRenderTarget->render();

        //deferred ambient occlusion
        if (visualOption.isAmbientOcclusionActivated) {
            ambientOcclusionManager->updateAOTexture(*camera);
        }

        transparentManager->updateTransparentTextures(*camera);
    }

    void Renderer3d::renderDebugSceneData(GeometryContainer& geometryContainer) {
        if (DEBUG_DISPLAY_MODELS_OCTREE) {
            if (debugModelOctree) {
                geometryContainer.removeGeometry(*debugModelOctree);
            }
            debugModelOctree = OctreeRenderer::createOctreeModel(*modelOctreeManager);
            geometryContainer.addGeometry(debugModelOctree);
        }

        if (DEBUG_DISPLAY_MODELS_BOUNDING_BOX) {
            modelSetDisplayer->drawBBox(geometryContainer);
        }

        if (DEBUG_DISPLAY_MODEL_BASE_BONES) {
            modelSetDisplayer->drawBaseBones(geometryContainer);
        }

        if (DEBUG_DISPLAY_LIGHTS_OCTREE) {
            lightManager->drawLightOctree(geometryContainer);
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

        lightingRenderTarget->render();
    }

    void Renderer3d::renderDebugFramebuffers(unsigned int& renderingOrder) {
        if (refreshDebugFramebuffers) {
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
                debugFramebuffer->prepareRendering(renderingOrder);
            }
        }
    }

    void Renderer3d::postUpdateScene() {
        ScopeProfiler sp(Profiler::graphic(), "postUpdateScene");

        modelOctreeManager->postRefreshOctreeables();
        lightManager->postUpdateVisibleLights();
    }

}
