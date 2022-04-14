#include <stdexcept>
#include <locale>

#include <scene/renderer3d/Renderer3d.h>
#include <scene/renderer3d/util/OpaqueMeshFilter.h>
#include <scene/renderer3d/util/OctreeRenderer.h>
#include <api/render/GenericRendererBuilder.h>
#include <api/render/shader/builder/ShaderBuilder.h>
#include <api/render/pipeline/PipelineContainer.h>
#include <api/render/target/NullRenderTarget.h>

namespace urchin {

    //debug parameters
    bool DEBUG_DISPLAY_DEPTH_BUFFER = false;
    bool DEBUG_DISPLAY_COLOR_BUFFER = false;
    bool DEBUG_DISPLAY_NORMAL_AMBIENT_BUFFER = false;
    bool DEBUG_DISPLAY_ILLUMINATED_SCENE_BUFFER = false; //TODO no influence on dependencies counter: why ?
    bool DEBUG_DISPLAY_AMBIENT_OCCLUSION_BUFFER = false;
    bool DEBUG_DISPLAY_MODELS_OCTREE = false;
    bool DEBUG_DISPLAY_MODELS_BOUNDING_BOX = false;
    bool DEBUG_DISPLAY_MODEL_BASE_BONES = false;
    bool DEBUG_DISPLAY_LIGHTS_OCTREE = false;

    Renderer3d::Renderer3d(RenderTarget& finalRenderTarget, std::shared_ptr<Camera> camera, const VisualConfig& visualConfig, I18nService& i18nService) :
            //scene properties
            finalRenderTarget(finalRenderTarget),
            sceneWidth(finalRenderTarget.getWidth()),
            sceneHeight(finalRenderTarget.getHeight()),
            paused(true),
            camera(std::move(camera)),

            //deferred rendering
            deferredRenderTarget(finalRenderTarget.isValidRenderTarget() ?
                    std::unique_ptr<RenderTarget>(new OffscreenRender("deferred rendering - first pass", RenderTarget::SHARED_DEPTH_ATTACHMENT)) :
                    std::unique_ptr<RenderTarget>(new NullRenderTarget(finalRenderTarget.getWidth(), finalRenderTarget.getHeight()))),
            modelOctreeManager(OctreeManager<Model>(ConfigService::instance().getFloatValue("model.octreeMinSize"))),
            modelSetDisplayer(ModelSetDisplayer(DisplayMode::DEFAULT_MODE)),
            fogContainer(FogContainer()),
            terrainContainer(TerrainContainer(visualConfig.getTerrainConfig(), *deferredRenderTarget)),
            waterContainer(WaterContainer(*deferredRenderTarget)),
            uiContainer(UiContainer(*deferredRenderTarget, i18nService)),
            geometryContainer(GeometryContainer(*deferredRenderTarget)),
            skyContainer(SkyContainer(*deferredRenderTarget)),
            lightManager(LightManager()),
            ambientOcclusionManager(AmbientOcclusionManager(visualConfig.getAmbientOcclusionConfig(), !finalRenderTarget.isValidRenderTarget())),
            transparentManager(TransparentManager(!finalRenderTarget.isValidRenderTarget(), lightManager)),
            shadowManager(ShadowManager(visualConfig.getShadowConfig(), lightManager, modelOctreeManager)),

            //lighting pass rendering
            lightingRenderTarget(finalRenderTarget.isValidRenderTarget() ?
                    std::unique_ptr<RenderTarget>(new OffscreenRender("deferred rendering - second pass", RenderTarget::NO_DEPTH_ATTACHMENT)) :
                    std::unique_ptr<RenderTarget>(new NullRenderTarget(finalRenderTarget.getWidth(), finalRenderTarget.getHeight()))),
            positioningData({}),
            visualOption({}),
            antiAliasingApplier(AntiAliasingApplier(visualConfig.getAntiAliasingConfig(), !finalRenderTarget.isValidRenderTarget())),
            isAntiAliasingActivated(visualConfig.isAntiAliasingActivated()),
            bloomEffectApplier(BloomEffectApplier(visualConfig.getBloomConfig(), finalRenderTarget)),

            //debug
            refreshDebugFramebuffers(true) {
        ScopeProfiler sp(Profiler::graphic(), "render3dInit");

        //scene properties
        assert(this->camera);
        this->camera->addObserver(this, Camera::PROJECTION_UPDATE);
        this->camera->initialize(sceneWidth, sceneHeight);

        //deferred rendering
        modelSetDisplayer.setupShader("model.vert.spv", "", "model.frag.spv", std::unique_ptr<ShaderConstants>(nullptr));
        modelSetDisplayer.setupMeshFilter(std::make_unique<OpaqueMeshFilter>());
        modelSetDisplayer.initialize(*deferredRenderTarget);
        shadowManager.addObserver(this, ShadowManager::NUMBER_SHADOW_MAPS_UPDATE);

        //lighting pass rendering
        visualOption.isShadowActivated = visualConfig.isShadowActivated();
        visualOption.isAmbientOcclusionActivated = visualConfig.isAmbientOcclusionActivated();
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
        camera->onResize(sceneWidth, sceneHeight);

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
        } else if (auto* model = dynamic_cast<Model*>(observable)) {
            if (notificationType == Model::ANIMATION_STARTED) {
                modelsAnimated.insert(model);
            }
        }
    }

    const OctreeManager<Model>& Renderer3d::getModelOctreeManager() const {
        return modelOctreeManager;
    }

    const ModelSetDisplayer& Renderer3d::getModelSetDisplayer() const {
        return modelSetDisplayer;
    }

    FogContainer& Renderer3d::getFogContainer() {
        return fogContainer;
    }

    TerrainContainer& Renderer3d::getTerrainContainer() {
        return terrainContainer;
    }

    WaterContainer& Renderer3d::getWaterContainer() {
        return waterContainer;
    }

    UiContainer& Renderer3d::get3dUiContainer() {
        return uiContainer;
    }

    GeometryContainer& Renderer3d::getGeometryContainer() {
        return geometryContainer;
    }

    SkyContainer& Renderer3d::getSkyContainer() {
        return skyContainer;
    }

    LightManager& Renderer3d::getLightManager() {
        return lightManager;
    }

    ShadowManager& Renderer3d::getShadowManager() {
        return shadowManager;
    }

    void Renderer3d::activateShadow(bool isShadowActivated) {
        if (visualOption.isShadowActivated != isShadowActivated) {
            visualOption.isShadowActivated = isShadowActivated;

            createOrUpdateLightingPass();
            shadowManager.forceUpdateAllShadowMaps();
        }
    }

    AmbientOcclusionManager& Renderer3d::getAmbientOcclusionManager() {
        return ambientOcclusionManager;
    }

    void Renderer3d::activateAmbientOcclusion(bool isAmbientOcclusionActivated) {
        if (visualOption.isAmbientOcclusionActivated != isAmbientOcclusionActivated) {
            visualOption.isAmbientOcclusionActivated = isAmbientOcclusionActivated;
            createOrUpdateLightingPass();
        }
    }

    TransparentManager& Renderer3d::getTransparentManager() {
        return transparentManager;
    }

    AntiAliasingApplier& Renderer3d::getAntiAliasingApplier() {
        return antiAliasingApplier;
    }

    BloomEffectApplier& Renderer3d::getBloomEffectApplier() {
        return bloomEffectApplier;
    }

    void Renderer3d::activateAntiAliasing(bool isAntiAliasingActivated) {
        if (this->isAntiAliasingActivated != isAntiAliasingActivated) {
            this->isAntiAliasingActivated = isAntiAliasingActivated;
            createOrUpdateLightingPass();
        }
    }

    void Renderer3d::onCameraProjectionUpdate() {
        ScopeProfiler sp(Profiler::graphic(), "render3dProjUp");

        uiContainer.onCameraProjectionUpdate(*camera);
        shadowManager.onCameraProjectionUpdate(*camera);
        ambientOcclusionManager.onCameraProjectionUpdate(*camera);
        transparentManager.onCameraProjectionUpdate(*camera);
        refreshDebugFramebuffers = true;
    }

    Camera& Renderer3d::getCamera() const {
        return *camera;
    }

    void Renderer3d::addModel(std::shared_ptr<Model> model) {
        if (model) {
            ScopeProfiler sp(Profiler::graphic(), "addModel");

            registerModelForAnimation(*model);
            modelOctreeManager.addOctreeable(std::move(model));
        }
    }

    std::shared_ptr<Model> Renderer3d::removeModel(Model* model) {
        if (model) {
            shadowManager.removeModel(model);
            transparentManager.removeModel(model);
            modelSetDisplayer.removeModel(model);
            unregisterModelForAnimation(*model);
            return modelOctreeManager.removeOctreeable(model);
        }
        return std::shared_ptr<Model>(nullptr);
    }

    /**
     * Pre-warm the models. Pre-warming avoid low frame rate on the first frame.
     * This method must be called once all the models and the camera have been setup.
     */
    void Renderer3d::preWarmModels() {
        updateScene(0.0f);
        postUpdateScene();
    }

    bool Renderer3d::isPaused() const {
        return paused;
    }

    bool Renderer3d::onKeyPress(unsigned int key) {
        bool propagateEvent = true;
        if (!paused) {
            propagateEvent = camera->onKeyPress(key);
            if (propagateEvent) {
                propagateEvent = uiContainer.onKeyPress(key);
            }
        }
        return propagateEvent;
    }

    bool Renderer3d::onKeyRelease(unsigned int key) {
        bool propagateEvent = true;
        if (!paused) {
            propagateEvent = camera->onKeyRelease(key);
            if (propagateEvent) {
                propagateEvent = uiContainer.onKeyRelease(key);
            }
        }
        return propagateEvent;
    }

    bool Renderer3d::onChar(char32_t unicodeCharacter) {
        return uiContainer.onChar(unicodeCharacter);
    }

    bool Renderer3d::onMouseMove(double mouseX, double mouseY) {
        bool propagateEvent = true;
        if (!paused) {
            camera->onMouseMove(mouseX, mouseY); //ignore 'propagateEvent' on purpose: UI must stay aware of event even when camera move
            propagateEvent = uiContainer.onMouseMove(mouseX, mouseY);
        }
        return propagateEvent;
    }

    bool Renderer3d::onScroll(double offsetY) {
        return uiContainer.onScroll(offsetY);
    }

    void Renderer3d::onDisable() {
        //nothing to do
    }

    void Renderer3d::pause() {
        paused = true;
    }

    void Renderer3d::unpause() {
        camera->resetPreviousMousePosition();
        paused = false;
    }

    void Renderer3d::prepareRendering(std::uint64_t frameIndex, float dt, unsigned int& screenRenderingOrder) {
        ScopeProfiler sp(Profiler::graphic(), "pre3dRendering");

        updateScene(dt);
        deferredRendering(frameIndex, dt);
        lightingPassRendering(frameIndex);
        if (isAntiAliasingActivated) {
            unsigned int numDependenciesToAATexture = 2 /* bloom pre-filter & bloom combine (screen target) */;
            antiAliasingApplier.applyAntiAliasing(frameIndex, numDependenciesToAATexture);
        }
        bloomEffectApplier.applyBloom(frameIndex, screenRenderingOrder);

        screenRenderingOrder++;
        renderDebugFramebuffers(screenRenderingOrder);

        postUpdateScene();
        PipelineContainer::instance().cleanPipelines();
    }

    void Renderer3d::registerModelForAnimation(Model& model) {
        model.addObserver(this, Model::ANIMATION_STARTED);
        if (model.isAnimated()) {
            modelsAnimated.insert(&model);
        }
    }

    void Renderer3d::unregisterModelForAnimation(Model& model) {
        modelsAnimated.erase(&model);
        model.removeObserver(this, Model::ANIMATION_STARTED);
    }

    void Renderer3d::createOrUpdateLightingPass() {
        //deferred rendering
        diffuseTexture = Texture::build("diffuse", sceneWidth, sceneHeight, TextureFormat::RGBA_8_INT, nullptr);
        normalAndAmbientTexture = Texture::build("normal and ambient", sceneWidth, sceneHeight, TextureFormat::RGBA_8_INT, nullptr);
        if (deferredRenderTarget && deferredRenderTarget->isValidRenderTarget()) {
            auto* deferredOffscreenRenderTarget = static_cast<OffscreenRender*>(deferredRenderTarget.get());
            deferredOffscreenRenderTarget->resetOutputTextures();
            deferredOffscreenRenderTarget->addOutputTexture(diffuseTexture);
            deferredOffscreenRenderTarget->addOutputTexture(normalAndAmbientTexture);
            deferredOffscreenRenderTarget->initialize();
        }

        //lighting pass rendering
        lightingPassTexture = Texture::build("lighted scene", sceneWidth, sceneHeight, TextureFormat::B10G11R11_FLOAT, nullptr);
        if (lightingRenderTarget && lightingRenderTarget->isValidRenderTarget()) {
            auto* offscreenLightingRenderTarget = static_cast<OffscreenRender*>(lightingRenderTarget.get());
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
        lightManager.setupLightingRenderer(lightingRendererBuilder); //binding 2
        shadowManager.setupLightingRenderer(lightingRendererBuilder); //binding 3 & 4
        fogContainer.setupLightingRenderer(lightingRendererBuilder); //binding 5

        std::vector<std::shared_ptr<TextureReader>> shadowMapTextureReaders;
        for (unsigned int i = 0; i < shadowManager.getMaxShadowLights(); ++i) {
            shadowMapTextureReaders.push_back(TextureReader::build(shadowManager.getEmptyShadowMapTexture(), TextureParam::buildNearest()));
        }
        lightingRenderer = lightingRendererBuilder
                ->addUniformTextureReader(TextureReader::build(deferredRenderTarget->getDepthTexture(), TextureParam::buildNearest())) //binding 6
                ->addUniformTextureReader(TextureReader::build(diffuseTexture, TextureParam::buildNearest())) //binding 7
                ->addUniformTextureReader(TextureReader::build(normalAndAmbientTexture, TextureParam::buildNearest())) //binding 8
                ->addUniformTextureReader(TextureReader::build(Texture::buildEmptyGreyscale("empty AO"), TextureParam::buildNearest())) //binding 9 - ambient occlusion
                ->addUniformTextureReader(TextureReader::build(Texture::buildEmptyRgba("transparent: empty accumulation"), TextureParam::buildNearest())) //binding 10 - transparency: accumulation
                ->addUniformTextureReader(TextureReader::build(Texture::buildEmptyGreyscale("transparent: empty reveal"), TextureParam::buildNearest())) //binding 11 - transparency: reveal
                ->addUniformTextureReaderArray(shadowMapTextureReaders) //binding 12
                ->build();
        ambientOcclusionManager.onTextureUpdate(deferredRenderTarget->getDepthTexture(), normalAndAmbientTexture);
        transparentManager.onTextureUpdate(deferredRenderTarget->getDepthTexture());

        //post process rendering
        if (isAntiAliasingActivated) {
            antiAliasingApplier.onTextureUpdate(lightingPassTexture);
        }
        bloomEffectApplier.onTextureUpdate(isAntiAliasingActivated ? antiAliasingApplier.getOutputTexture() : lightingPassTexture);

        refreshDebugFramebuffers = true;
    }

    void Renderer3d::createOrUpdateLightingShader() {
        LightingShaderConst lightingConstData{};
        lightingConstData.maxLights = lightManager.getMaxLights();
        lightingConstData.maxShadowLights = shadowManager.getMaxShadowLights();
        lightingConstData.numberShadowMaps = shadowManager.getConfig().nbShadowMaps;
        lightingConstData.shadowMapConstantBias = shadowManager.getShadowMapConstantBias();
        lightingConstData.shadowMapSlopeBiasFactor = shadowManager.getShadowMapSlopeBiasFactor();
        lightingConstData.maxEmissiveFactor = Material::MAX_EMISSIVE_FACTOR;
        std::vector<std::size_t> variablesSize = {
                sizeof(LightingShaderConst::maxLights),
                sizeof(LightingShaderConst::maxShadowLights),
                sizeof(LightingShaderConst::numberShadowMaps),
                sizeof(LightingShaderConst::shadowMapConstantBias),
                sizeof(LightingShaderConst::shadowMapSlopeBiasFactor),
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
        camera->refreshCameraView(dt);

        //refresh models in octree
        modelOctreeManager.refreshOctreeables();

        //determine model visible on scene
        modelsInFrustum.clear();
        modelOctreeManager.getOctreeablesIn(camera->getFrustum(), modelsInFrustum);

        //determine visible lights on scene
        lightManager.updateVisibleLights(camera->getFrustum());

        //determine models producing shadow on scene
        if (visualOption.isShadowActivated) {
            shadowManager.updateVisibleModels(camera->getFrustum());
        }

        //animate models
        std::erase_if(modelsAnimated, [](const Model* modelAnimated){ return !modelAnimated->isAnimated(); });
        for (Model* modelAnimated : modelsAnimated) {
            modelAnimated->updateAnimation(dt);
        }

        //update models (must be done after the animations because it can change the ModelDisplayable#computeInstanceId)
        modelSetDisplayer.updateModels(modelsInFrustum);
        transparentManager.updateModels(modelsInFrustum);
    }

    /**
     * First pass of deferred shading algorithm.
     * Render depth, color, normal, etc. into buffers.
     */
    void Renderer3d::deferredRendering(std::uint64_t frameIndex, float dt) {
        ScopeProfiler sp(Profiler::graphic(), "deferredRender");

        //deferred shadow map
        if (visualOption.isShadowActivated) {
            unsigned int numDependenciesToShadowMaps = 1 /* second pass */;
            shadowManager.updateShadowMaps(frameIndex, numDependenciesToShadowMaps);
        }

        //deferred scene (depth, color, normal, ambient...)
        deferredRenderTarget->disableAllRenderers();

        unsigned int deferredRenderingOrder = 0;
        skyContainer.prepareRendering(deferredRenderingOrder, camera->getProjectionViewMatrix(), camera->getPosition());

        deferredRenderingOrder++;
        modelSetDisplayer.prepareRendering(deferredRenderingOrder, camera->getProjectionViewMatrix());

        deferredRenderingOrder++;
        terrainContainer.prepareRendering(deferredRenderingOrder, *camera, dt);

        deferredRenderingOrder++;
        waterContainer.prepareRendering(deferredRenderingOrder, *camera, fogContainer, dt);

        deferredRenderingOrder++;
        uiContainer.prepareRendering(dt, deferredRenderingOrder, camera->getProjectionViewMatrix());

        renderDebugSceneData(geometryContainer);
        deferredRenderingOrder++;
        geometryContainer.prepareRendering(deferredRenderingOrder, camera->getProjectionViewMatrix());

        deferredRenderTarget->render(frameIndex, computeDependenciesToFirstPassOutput());

        //deferred ambient occlusion
        if (visualOption.isAmbientOcclusionActivated) {
            unsigned int numDependenciesToAOTexture = 1 /* second pass */;
            if (DEBUG_DISPLAY_AMBIENT_OCCLUSION_BUFFER) {
                numDependenciesToAOTexture++;
            }
            ambientOcclusionManager.updateAOTexture(frameIndex, numDependenciesToAOTexture, *camera);
        }

        unsigned int numDependenciesToTransparentTextures = 1 /* second pass */;
        transparentManager.updateTransparentTextures(frameIndex, numDependenciesToTransparentTextures, *camera);
    }

    unsigned int Renderer3d::computeDependenciesToFirstPassOutput() const {
        unsigned int numDependenciesToFirstPassOutput = 2 /* transparent/accum & second pass */;
        if (visualOption.isAmbientOcclusionActivated) {
            numDependenciesToFirstPassOutput += 3 /* AO raw texture & AO vertical filter & AO horizontal filter */;
        }
        if (DEBUG_DISPLAY_DEPTH_BUFFER || DEBUG_DISPLAY_COLOR_BUFFER || DEBUG_DISPLAY_NORMAL_AMBIENT_BUFFER) {
            numDependenciesToFirstPassOutput++;
        }
        return numDependenciesToFirstPassOutput;
    }

    void Renderer3d::renderDebugSceneData(GeometryContainer& geometryContainer) {
        if (DEBUG_DISPLAY_MODELS_OCTREE) {
            if (debugModelOctree) {
                geometryContainer.removeGeometry(*debugModelOctree);
            }
            debugModelOctree = OctreeRenderer::createOctreeModel(modelOctreeManager);
            geometryContainer.addGeometry(debugModelOctree);
        }

        if (DEBUG_DISPLAY_MODELS_BOUNDING_BOX) {
            modelSetDisplayer.drawBBox(geometryContainer);
        }

        if (DEBUG_DISPLAY_MODEL_BASE_BONES) {
            modelSetDisplayer.drawBaseBones(geometryContainer);
        }

        if (DEBUG_DISPLAY_LIGHTS_OCTREE) {
            lightManager.drawLightOctree(geometryContainer);
        }
    }

    /**
     * Second pass of deferred shading algorithm.
     * Compute lighting in pixel shader and render the scene to screen.
     */
    void Renderer3d::lightingPassRendering(std::uint64_t frameIndex) {
        ScopeProfiler sp(Profiler::graphic(), "lightPassRender");

        positioningData.inverseProjectionViewMatrix = camera->getProjectionViewInverseMatrix();
        positioningData.viewPosition = camera->getPosition();
        constexpr std::size_t positioningDataUniformIndex = 0;
        lightingRenderer->updateUniformData(positioningDataUniformIndex, &positioningData);

        constexpr std::size_t lightsDataUniformIndex = 2;
        lightManager.loadVisibleLights(*lightingRenderer, lightsDataUniformIndex);

        constexpr std::size_t fogUniformIndex = 5;
        fogContainer.loadFog(*lightingRenderer, fogUniformIndex);

        if (visualOption.isAmbientOcclusionActivated) {
            constexpr std::size_t ambientOcclusionTexUnit = 3;
            ambientOcclusionManager.loadAOTexture(*lightingRenderer, ambientOcclusionTexUnit);
        }

        constexpr std::size_t transparentAccumulationTexUnit = 4;
        constexpr std::size_t transparentRevealTexUnit = 5;
        transparentManager.loadTransparentTextures(*lightingRenderer, transparentAccumulationTexUnit, transparentRevealTexUnit);

        if (visualOption.isShadowActivated) {
            constexpr std::size_t shadowMapTexUnit = 6;
            shadowManager.loadShadowMaps(*lightingRenderer, shadowMapTexUnit);
        }

        unsigned int numDependenciesToSecondPassOutput;
        if (isAntiAliasingActivated) {
            numDependenciesToSecondPassOutput = 1 /* anti aliasing */;
        } else {
            numDependenciesToSecondPassOutput = 2 /* bloom pre-filter && bloom combine (screen target) */;
        }
        lightingRenderTarget->render(frameIndex, numDependenciesToSecondPassOutput);
    }

    void Renderer3d::renderDebugFramebuffers(unsigned int renderingOrder) {
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

            if (DEBUG_DISPLAY_AMBIENT_OCCLUSION_BUFFER && visualOption.isAmbientOcclusionActivated) {
                auto textureRenderer = std::make_unique<TextureRenderer>(ambientOcclusionManager.getAmbientOcclusionTexture(), TextureRenderer::INVERSE_GRAYSCALE_VALUE);
                textureRenderer->setPosition(TextureRenderer::CENTER_X, TextureRenderer::BOTTOM);
                textureRenderer->initialize("[DEBUG] ambient occlusion texture", finalRenderTarget, sceneWidth, sceneHeight, 0.0f, 0.05f);
                debugFramebuffers.emplace_back(std::move(textureRenderer));
            }
        } else {
            for (const auto& debugFramebuffer : debugFramebuffers) {
                debugFramebuffer->prepareRendering(renderingOrder);
            }
        }
    }

    void Renderer3d::postUpdateScene() {
        ScopeProfiler sp(Profiler::graphic(), "postUpdateScene");

        modelOctreeManager.postRefreshOctreeables();
        lightManager.postUpdateVisibleLights();
    }

}
