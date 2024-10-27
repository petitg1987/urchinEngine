#include <stdexcept>

#include <scene/renderer3d/Renderer3d.h>
#include <scene/renderer3d/OpaqueMeshFilter.h>
#include <graphics/render/shader/ShaderBuilder.h>
#include <graphics/render/GenericRendererBuilder.h>

namespace urchin {

    //debug parameters
    const bool DEBUG_DISPLAY_DEPTH_BUFFER = False();
    const bool DEBUG_DISPLAY_ALBEDO_EMISSIVE_BUFFER = False();
    const bool DEBUG_DISPLAY_NORMAL_AMBIENT_BUFFER = False();
    const bool DEBUG_DISPLAY_MATERIAL_BUFFER = False();
    const bool DEBUG_DISPLAY_ILLUMINATED_BUFFER = False();
    const bool DEBUG_DISPLAY_TRANSPARENT_BUFFER = False();
    const bool DEBUG_DISPLAY_AMBIENT_OCCLUSION_BUFFER = False();
    const bool DEBUG_DISPLAY_MODELS_OCCLUSION_CULLER_DATA = False();
    const bool DEBUG_DISPLAY_MODELS_BOUNDING_BOX = False();
    const bool DEBUG_DISPLAY_MODEL_BASE_BONES = False();
    const bool DEBUG_DISPLAY_LIGHTS_OCTREE = False();

    Renderer3d::Renderer3d(float gammaFactor, RenderTarget& finalRenderTarget, std::shared_ptr<Camera> camera, const VisualConfig& visualConfig, I18nService& i18nService) :
            Renderer(gammaFactor),

            //scene properties
            finalRenderTarget(finalRenderTarget),
            sceneWidth(finalRenderTarget.getWidth()),
            sceneHeight(finalRenderTarget.getHeight()),
            paused(true),
            camera(std::move(camera)),
            renderingScale(visualConfig.getRenderingScale()),

            //deferred first pass
            deferredFirstPassRenderTarget(std::unique_ptr<OffscreenRender>(new OffscreenRender("deferred rendering - first pass", finalRenderTarget.isTestMode(), RenderTarget::SHARED_DEPTH_ATTACHMENT))),
            modelOcclusionCuller(ModelOcclusionCuller()),
            modelSetDisplayer(ModelSetDisplayer(DisplayMode::DEFAULT_MODE)),
            fogContainer(FogContainer()),
            terrainContainer(TerrainContainer(visualConfig.getTerrainConfig(), *deferredFirstPassRenderTarget)),
            waterContainer(WaterContainer(*deferredFirstPassRenderTarget)),
            uiContainer(UiContainer(*deferredFirstPassRenderTarget, i18nService)),
            geometryContainer(GeometryContainer(*deferredFirstPassRenderTarget)),
            skyContainer(SkyContainer(*deferredFirstPassRenderTarget)),
            lightManager(LightManager()),
            ambientOcclusionManager(AmbientOcclusionManager(visualConfig.getAmbientOcclusionConfig(), finalRenderTarget.isTestMode())),
            transparentManager(TransparentManager(finalRenderTarget.isTestMode(), lightManager)),
            shadowManager(ShadowManager(visualConfig.getShadowConfig(), lightManager, modelOcclusionCuller)),

            //deferred second pass
            deferredSecondPassRenderTarget(std::unique_ptr<RenderTarget>(new OffscreenRender("deferred rendering - second pass", finalRenderTarget.isTestMode(), RenderTarget::NO_DEPTH_ATTACHMENT))),
            positioningData({}),
            sceneInfo({}),
            antiAliasingApplier(AntiAliasingApplier(visualConfig.getAntiAliasingConfig(), finalRenderTarget.isTestMode())),
            isAntiAliasingActivated(visualConfig.isAntiAliasingActivated()),
            bloomEffectApplier(BloomEffectApplier(visualConfig.getBloomConfig(), finalRenderTarget.isTestMode(), getGammaFactor())),
            reflectionApplier(ReflectionApplier(visualConfig.getReflectionConfig(), finalRenderTarget)),
            isReflectionActivated(visualConfig.isReflectionActivated()),

            //debug
            refreshDebugFramebuffers(true) {
        ScopeProfiler sp(Profiler::graphic(), "render3dInit");

        //scene properties
        assert(this->camera);
        this->camera->addObserver(this, Camera::PROJECTION_UPDATE);
        this->camera->initialize(sceneWidth, sceneHeight);

        //deferred passes
        modelSetDisplayer.setupShader("model.vert.spv", "model.frag.spv", std::unique_ptr<ShaderConstants>(nullptr));
        modelSetDisplayer.setupMeshFilter(std::make_unique<OpaqueMeshFilter>());
        modelSetDisplayer.initialize(*deferredFirstPassRenderTarget);
        ambientOcclusionManager.addObserver(this, AmbientOcclusionManager::AMBIENT_OCCLUSION_STRENGTH_UPDATE);
        shadowManager.addObserver(this, ShadowManager::NUMBER_SHADOW_MAPS_UPDATE);

        sceneInfo.sceneSize = Point2<float>((float)sceneWidth, (float)sceneHeight);
        sceneInfo.isShadowActivated = visualConfig.isShadowActivated();
        sceneInfo.isAmbientOcclusionActivated = visualConfig.isAmbientOcclusionActivated();

        createOrUpdateDeferredPasses();
    }

    Renderer3d::~Renderer3d() {
        deferredSecondPassRenderTarget->cleanup();
        deferredFirstPassRenderTarget->cleanup();
    }

    void Renderer3d::onResize(unsigned int sceneWidth, unsigned int sceneHeight) {
        //scene properties
        this->sceneWidth = sceneWidth;
        this->sceneHeight = sceneHeight;

        //camera
        camera->onResize(sceneWidth, sceneHeight);

        createOrUpdateDeferredPasses();
    }

    void Renderer3d::notify(Observable* observable, int notificationType) {
        if (dynamic_cast<AmbientOcclusionManager*>(observable)) {
            if (notificationType == AmbientOcclusionManager::AMBIENT_OCCLUSION_STRENGTH_UPDATE) {
                createOrUpdateDeferredPasses();
            }
        } else if (dynamic_cast<ShadowManager*>(observable)) {
            if (notificationType == ShadowManager::NUMBER_SHADOW_MAPS_UPDATE) {
                createOrUpdateDeferredPasses();
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

    const ModelOcclusionCuller& Renderer3d::getModelOcclusionCuller() const {
        return modelOcclusionCuller;
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

    void Renderer3d::activateShadow(bool isShadowActivated) {
        if (sceneInfo.isShadowActivated != isShadowActivated) {
            sceneInfo.isShadowActivated = isShadowActivated;
            createOrUpdateDeferredPasses();
        }
    }

    bool Renderer3d::isShadowActivated() const {
        return sceneInfo.isShadowActivated;
    }

    ShadowManager& Renderer3d::getShadowManager() {
        return shadowManager;
    }

    void Renderer3d::activateAmbientOcclusion(bool isAmbientOcclusionActivated) {
        if (sceneInfo.isAmbientOcclusionActivated != isAmbientOcclusionActivated) {
            sceneInfo.isAmbientOcclusionActivated = isAmbientOcclusionActivated;
            createOrUpdateDeferredPasses();
        }
    }

    bool Renderer3d::isAmbientOcclusionActivated() const {
        return sceneInfo.isAmbientOcclusionActivated;
    }

    AmbientOcclusionManager& Renderer3d::getAmbientOcclusionManager() {
        return ambientOcclusionManager;
    }

    TransparentManager& Renderer3d::getTransparentManager() {
        return transparentManager;
    }

    void Renderer3d::activateAntiAliasing(bool isAntiAliasingActivated) {
        this->isAntiAliasingActivated = isAntiAliasingActivated;
    }

    AntiAliasingApplier& Renderer3d::getAntiAliasingApplier() {
        return antiAliasingApplier;
    }

    BloomEffectApplier& Renderer3d::getBloomEffectApplier() {
        return bloomEffectApplier;
    }

    void Renderer3d::activateReflection(bool isReflectionActivated) {
        this->isReflectionActivated = isReflectionActivated;
    }

    ReflectionApplier& Renderer3d::getReflectionApplier() {
        return reflectionApplier;
    }

    void Renderer3d::updateRenderingScale(float renderingScale) {
        if (this->renderingScale != renderingScale) {
            this->renderingScale = renderingScale;
            createOrUpdateDeferredPasses();
        }
    }

    void Renderer3d::applyUpdatedGammaFactor() {
        bloomEffectApplier.onGammaFactorUpdate(getGammaFactor());
    }

    void Renderer3d::onCameraProjectionUpdate() {
        ScopeProfiler sp(Profiler::graphic(), "render3dProjUp");

        uiContainer.onCameraProjectionUpdate(*camera);
        shadowManager.onCameraProjectionUpdate(*camera);
        ambientOcclusionManager.onCameraProjectionUpdate(*camera);
        transparentManager.onCameraProjectionUpdate(*camera);
        reflectionApplier.onCameraProjectionUpdate(*camera);
        refreshDebugFramebuffers = true;
    }

    Camera& Renderer3d::getCamera() const {
        return *camera;
    }

    void Renderer3d::addModel(std::shared_ptr<Model> model) {
        if (model) {
            ScopeProfiler sp(Profiler::graphic(), "addModel");

            registerModelForAnimation(*model);
            modelOcclusionCuller.addModel(std::move(model));
        }
    }

    std::shared_ptr<Model> Renderer3d::removeModel(Model* model) {
        if (model) {
            shadowManager.removeModel(model);
            transparentManager.removeModel(model);
            modelSetDisplayer.removeModel(model);
            unregisterModelForAnimation(*model);
            return modelOcclusionCuller.removeModel(model);
        }
        return {nullptr};
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

    bool Renderer3d::onKeyPress(InputDeviceKey key) {
        bool propagateEvent = true;
        if (!paused) {
            propagateEvent = camera->onKeyPress(key);
            if (propagateEvent) {
                propagateEvent = uiContainer.onKeyPress(key);
            }
        }
        return propagateEvent;
    }

    bool Renderer3d::onKeyRelease(InputDeviceKey key) {
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

    bool Renderer3d::onMouseMove(double mouseX, double mouseY, double deltaMouseX, double deltaMouseY) {
        bool propagateEvent = true;
        if (!paused) {
            camera->onMouseMove(deltaMouseX, deltaMouseY); //ignore 'propagateEvent' on purpose: UI must stay aware of event even when camera move
            propagateEvent = uiContainer.onMouseMove(mouseX, mouseY, deltaMouseX, deltaMouseY);
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
        paused = false;
    }

    void Renderer3d::prepareRendering(std::uint32_t frameIndex, float dt, unsigned int& screenRenderingOrder) {
        ScopeProfiler sp(Profiler::graphic(), "pre3dRendering");

        updateScene(dt);

        renderDeferredFirstPass(frameIndex, dt);
        renderDeferredSecondPass(frameIndex);

        unsigned int numDependenciesToTransparentTextures = isAntiAliasingActivated ? 1 /* AA */ : 2 /* bloom pre-filter & bloom combine */;
        if (DEBUG_DISPLAY_TRANSPARENT_BUFFER) {
            numDependenciesToTransparentTextures += 1;
        }
        transparentManager.drawTransparentModels(frameIndex, numDependenciesToTransparentTextures, *camera);

        if (isAntiAliasingActivated) {
            unsigned int numDependenciesToAATexture = 2; //bloom pre-filter & bloom combine
            antiAliasingApplier.applyAntiAliasing(frameIndex, numDependenciesToAATexture);
        }

        unsigned int numDependenciesToBloomCombineTexture = isReflectionActivated ? 2 /* reflection color && reflection combine */ : 1 /* screen */;
        bloomEffectApplier.applyBloom(frameIndex, numDependenciesToBloomCombineTexture);

        if (isReflectionActivated) {
            reflectionApplier.applyReflection(frameIndex, screenRenderingOrder, *camera);
        }

        screenRenderingOrder++;
        renderDebugFramebuffers(screenRenderingOrder);

        postUpdateScene();
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

    void Renderer3d::createOrUpdateDeferredPasses() {
        unsigned int renderingSceneWidth = MathFunction::roundToUInt((float)sceneWidth * renderingScale);
        unsigned int renderingSceneHeight = MathFunction::roundToUInt((float)sceneHeight * renderingScale);

        //deferred first pass
        albedoAndEmissiveTexture = Texture::build("albedo and emissive", renderingSceneWidth, renderingSceneHeight, VisualConfig::ALBEDO_AND_EMISSIVE_TEXTURE_FORMAT);
        normalAndAmbientTexture = Texture::build("normal and ambient", renderingSceneWidth, renderingSceneHeight, VisualConfig::NORMAL_AND_AMBIENT_TEXTURE_FORMAT);
        materialTexture = Texture::build("material", renderingSceneWidth, renderingSceneHeight, VisualConfig::MATERIAL_TEXTURE_FORMAT);
        if (deferredFirstPassRenderTarget) {
            auto* deferredFirstPassOffscreenRender = static_cast<OffscreenRender*>(deferredFirstPassRenderTarget.get());
            deferredFirstPassOffscreenRender->resetOutput();
            deferredFirstPassOffscreenRender->addOutputTexture(albedoAndEmissiveTexture);
            deferredFirstPassOffscreenRender->addOutputTexture(normalAndAmbientTexture);
            deferredFirstPassOffscreenRender->addOutputTexture(materialTexture);
            deferredFirstPassOffscreenRender->initialize();
        }

        //deferred second pass
        sceneInfo.sceneSize = Point2<float>((float)sceneWidth, (float)sceneHeight);
        illuminatedTexture = Texture::build("illuminated scene", renderingSceneWidth, renderingSceneHeight, VisualConfig::SCENE_TEXTURE_FORMAT);
        if (deferredSecondPassRenderTarget) {
            auto* deferredSecondPassOffscreenRender = static_cast<OffscreenRender*>(deferredSecondPassRenderTarget.get());
            deferredSecondPassOffscreenRender->resetOutput();
            deferredSecondPassOffscreenRender->addOutputTexture(illuminatedTexture);
            deferredSecondPassOffscreenRender->initialize();
        }

        createOrUpdateDeferredSecondPassShader();

        std::vector<Point2<float>> vertexCoord = {
                Point2<float>(-1.0f, -1.0f), Point2<float>(1.0f, -1.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(-1.0f, -1.0f), Point2<float>(1.0f, 1.0f), Point2<float>(-1.0f, 1.0f)
        };
        std::vector<Point2<float>> textureCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 0.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(0.0f, 1.0f)
        };

        auto deferredSecondPassRendererBuilder = GenericRendererBuilder::create("deferred rendering - second pass", *deferredSecondPassRenderTarget, *deferredSecondPassShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformData(POSITIONING_DATA_UNIFORM_BINDING, sizeof(positioningData), &positioningData)
                ->addUniformData(SCENE_INFO_UNIFORM_BINDING, sizeof(sceneInfo), &sceneInfo);
        lightManager.setupDeferredSecondPassRenderer(deferredSecondPassRendererBuilder, LIGHTS_DATA_UNIFORM_BINDING);
        shadowManager.setupDeferredSecondPassRenderer(deferredSecondPassRendererBuilder, SM_PROJ_VIEW_MATRICES_UNIFORM_BINDING, SM_DATA_UNIFORM_BINDING, SM_INFO_UNIFORM_BINDING);
        fogContainer.setupDeferredSecondPassRenderer(deferredSecondPassRendererBuilder, FOG_UNIFORM_BINDING);

        std::vector<std::shared_ptr<TextureReader>> shadowMapTextureReaders;
        shadowMapTextureReaders.reserve(shadowManager.getMaxShadowLights());
        for (unsigned int i = 0; i < shadowManager.getMaxShadowLights(); ++i) {
            shadowMapTextureReaders.push_back(TextureReader::build(shadowManager.getEmptyShadowMapTexture(), TextureParam::buildNearest()));
        }
        deferredSecondPassRenderer = deferredSecondPassRendererBuilder
                ->addUniformTextureReader(DEPTH_TEX_UNIFORM_BINDING, TextureReader::build(deferredFirstPassRenderTarget->getDepthTexture(), TextureParam::buildNearest()))
                ->addUniformTextureReader(ALBEDO_EMISSIVE_TEX_UNIFORM_BINDING, TextureReader::build(albedoAndEmissiveTexture, TextureParam::buildNearest()))
                ->addUniformTextureReader(NORMAL_AMBIENT_TEX_UNIFORM_BINDING, TextureReader::build(normalAndAmbientTexture, TextureParam::buildNearest()))
                ->addUniformTextureReader(MATERIAL_TEX_UNIFORM_BINDING, TextureReader::build(materialTexture, TextureParam::buildNearest()))
                ->addUniformTextureReader(AO_TEX_UNIFORM_BINDING, TextureReader::build(Texture::buildEmptyGreyscale("empty AO"), TextureParam::buildNearest()))
                ->addUniformTextureReaderArray(SM_TEX_UNIFORM_BINDING, shadowMapTextureReaders)
                ->addUniformTextureReader(SM_OFFSET_TEX_UNIFORM_BINDING, TextureReader::build(Texture::buildEmptyArrayRg("empty SM offset"), TextureParam::buildNearest()))
                ->build();

        refreshDebugFramebuffers = true;
    }

    void Renderer3d::createOrUpdateDeferredSecondPassShader() {
        DeferredSecondPassShaderConst deferredSecondPassConstData {
                .maxLights = lightManager.getMaxLights(),
                .ambientOcclusionStrength = ambientOcclusionManager.getAmbientOcclusionStrength(), //apply AO strength after AO blur to not lose color precision on 8bit texture
                .maxShadowLights = shadowManager.getMaxShadowLights(),
                .numberShadowMaps = shadowManager.getConfig().nbShadowMaps,
                .shadowMapConstantBias = shadowManager.getShadowMapConstantBias(),
                .shadowMapSlopeBiasFactor = shadowManager.getShadowMapSlopeBiasFactor(),
                .shadowMapOffsetTexSize = shadowManager.getShadowMapOffsetTexSize(),
                .maxEmissiveFactor = Material::MAX_EMISSIVE_FACTOR
        };
        std::vector<std::size_t> variablesSize = {
                sizeof(DeferredSecondPassShaderConst::maxLights),
                sizeof(DeferredSecondPassShaderConst::ambientOcclusionStrength),
                sizeof(DeferredSecondPassShaderConst::maxShadowLights),
                sizeof(DeferredSecondPassShaderConst::numberShadowMaps),
                sizeof(DeferredSecondPassShaderConst::shadowMapConstantBias),
                sizeof(DeferredSecondPassShaderConst::shadowMapSlopeBiasFactor),
                sizeof(DeferredSecondPassShaderConst::shadowMapOffsetTexSize),
                sizeof(DeferredSecondPassShaderConst::maxEmissiveFactor)
        };
        auto shaderConstants = std::make_unique<ShaderConstants>(variablesSize, &deferredSecondPassConstData);
        deferredSecondPassShader = ShaderBuilder::createShader("deferredSecondPass.vert.spv", "deferredSecondPass.frag.spv", std::move(shaderConstants), deferredSecondPassRenderTarget->isTestMode());
    }

    void Renderer3d::updateScene(float dt) {
        ScopeProfiler sp(Profiler::graphic(), "updateScene");

        //refresh input textures
        if (sceneInfo.isAmbientOcclusionActivated) {
            ambientOcclusionManager.refreshInputTextures(deferredFirstPassRenderTarget->getDepthTexture(), normalAndAmbientTexture);
        }

        std::shared_ptr<Texture> currentSceneTexture = illuminatedTexture;
        transparentManager.refreshInputTextures(deferredFirstPassRenderTarget->getDepthTexture(), currentSceneTexture);
        currentSceneTexture = transparentManager.getOutputTexture();

        if (isAntiAliasingActivated) {
            antiAliasingApplier.refreshInputTexture(transparentManager.getOutputTexture());
            currentSceneTexture = antiAliasingApplier.getOutputTexture();
        }

        bloomEffectApplier.refreshInputTexture(currentSceneTexture);
        currentSceneTexture = bloomEffectApplier.getOutputTexture();

        if (isReflectionActivated) {
            reflectionApplier.refreshInputTexture(deferredFirstPassRenderTarget->getDepthTexture(), normalAndAmbientTexture, materialTexture, currentSceneTexture);
        }

        //refresh the model occlusion culler
        modelOcclusionCuller.refresh();

        //determine model visible on scene
        modelsInFrustum.clear();
        modelOcclusionCuller.getModelsInFrustum(camera->getFrustum(), modelsInFrustum);

        //determine visible lights on scene
        lightManager.updateVisibleLights(camera->getFrustum());

        //determine models producing shadow on scene
        if (sceneInfo.isShadowActivated) {
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
     * Render depth, albedo, normal, etc. into buffers.
     */
    void Renderer3d::renderDeferredFirstPass(std::uint32_t frameIndex, float dt) {
        ScopeProfiler sp(Profiler::graphic(), "renFirstPass");

        //deferred shadow map
        if (sceneInfo.isShadowActivated) {
            unsigned int numDependenciesToShadowMaps = 1; //second pass
            shadowManager.updateShadowMaps(frameIndex, numDependenciesToShadowMaps);
        }

        //deferred scene (depth, albedo, normal, ambient...)
        deferredFirstPassRenderTarget->disableAllProcessors();

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

        deferredFirstPassRenderTarget->render(frameIndex, computeDependenciesToFirstPassOutput());

        //deferred ambient occlusion
        if (sceneInfo.isAmbientOcclusionActivated) {
            unsigned int numDependenciesToAOTexture = 1; //second pass
            if (DEBUG_DISPLAY_AMBIENT_OCCLUSION_BUFFER) {
                numDependenciesToAOTexture++;
            }
            ambientOcclusionManager.updateAOTexture(frameIndex, numDependenciesToAOTexture, *camera);
        }
    }

    unsigned int Renderer3d::computeDependenciesToFirstPassOutput() const {
        unsigned int numDependenciesToFirstPassOutput = 1; //second pass
        if (sceneInfo.isAmbientOcclusionActivated) {
            if (ambientOcclusionManager.getConfig().isBlurActivated) {
                numDependenciesToFirstPassOutput += 3; //AO texture & AO blur (vertical & horizontal)
            } else {
                numDependenciesToFirstPassOutput += 1; //AO texture
            }
        }
        numDependenciesToFirstPassOutput += 1; //transparent
        if (isReflectionActivated) {
            numDependenciesToFirstPassOutput += 3; //reflection color & reflection blur (vertical & horizontal)
        }
        if (DEBUG_DISPLAY_DEPTH_BUFFER || DEBUG_DISPLAY_ALBEDO_EMISSIVE_BUFFER || DEBUG_DISPLAY_NORMAL_AMBIENT_BUFFER || DEBUG_DISPLAY_MATERIAL_BUFFER) {
            numDependenciesToFirstPassOutput++; //bloom combine (screen target)
        }
        return numDependenciesToFirstPassOutput;
    }

    void Renderer3d::renderDebugSceneData(GeometryContainer& geometryContainer) {
        if (DEBUG_DISPLAY_MODELS_OCCLUSION_CULLER_DATA) {
            if (debugOcclusionCullerGeometries) {
                geometryContainer.removeGeometry(*debugOcclusionCullerGeometries);
            }
            debugOcclusionCullerGeometries = modelOcclusionCuller.createDebugGeometries();
            geometryContainer.addGeometry(debugOcclusionCullerGeometries);
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
     * Compute lighting in pixel shader to render the scene.
     */
    void Renderer3d::renderDeferredSecondPass(std::uint32_t frameIndex) {
        ScopeProfiler sp(Profiler::graphic(), "renSecondPass");

        positioningData.inverseProjectionViewMatrix = camera->getProjectionViewInverseMatrix();
        positioningData.viewPosition = camera->getPosition();
        deferredSecondPassRenderer->updateUniformData(POSITIONING_DATA_UNIFORM_BINDING, &positioningData);

        lightManager.loadVisibleLights(*deferredSecondPassRenderer, LIGHTS_DATA_UNIFORM_BINDING);

        fogContainer.loadFog(*deferredSecondPassRenderer, FOG_UNIFORM_BINDING);

        if (sceneInfo.isAmbientOcclusionActivated) {
            ambientOcclusionManager.loadAOTexture(*deferredSecondPassRenderer, AO_TEX_UNIFORM_BINDING);
        }

        if (sceneInfo.isShadowActivated) {
            shadowManager.loadShadowMaps(*deferredSecondPassRenderer, SM_PROJ_VIEW_MATRICES_UNIFORM_BINDING, SM_DATA_UNIFORM_BINDING, SM_INFO_UNIFORM_BINDING,
                                         SM_TEX_UNIFORM_BINDING, SM_OFFSET_TEX_UNIFORM_BINDING);
        }

        deferredSecondPassRenderTarget->render(frameIndex, computeDependenciesToSecondPassOutput());
    }

    unsigned int Renderer3d::computeDependenciesToSecondPassOutput() const {
        unsigned int numDependenciesToSecondPassOutput = 0; //transparent use second pass output but outside render pass with barrier (no need semaphore)
        if (DEBUG_DISPLAY_ILLUMINATED_BUFFER) {
            numDependenciesToSecondPassOutput++; //bloom combine (screen target)
        }
        return numDependenciesToSecondPassOutput;
    }

    void Renderer3d::renderDebugFramebuffers(unsigned int renderingOrder) {
        if (refreshDebugFramebuffers) {
            debugFramebuffers.clear();
            refreshDebugFramebuffers = false;

            if (DEBUG_DISPLAY_DEPTH_BUFFER) {
                auto textureRenderer = std::make_unique<TextureRenderer>(deferredFirstPassRenderTarget->getDepthTexture(), TextureRenderer::GRAYSCALE_VALUE);
                textureRenderer->setPosition(TextureRenderer::LEFT, TextureRenderer::TOP);
                textureRenderer->initialize("[DEBUG] depth texture", finalRenderTarget, sceneWidth, sceneHeight, 0.95f, 1.0f);
                debugFramebuffers.emplace_back(std::move(textureRenderer));
            }

            if (DEBUG_DISPLAY_ALBEDO_EMISSIVE_BUFFER) {
                auto textureRenderer = std::make_unique<TextureRenderer>(albedoAndEmissiveTexture, TextureRenderer::DEFAULT_VALUE);
                textureRenderer->setPosition(TextureRenderer::CENTER_X, TextureRenderer::TOP);
                textureRenderer->initialize("[DEBUG] albedo/emissive texture", finalRenderTarget, sceneWidth, sceneHeight);
                debugFramebuffers.emplace_back(std::move(textureRenderer));
            }

            if (DEBUG_DISPLAY_NORMAL_AMBIENT_BUFFER) {
                auto textureRenderer = std::make_unique<TextureRenderer>(normalAndAmbientTexture, TextureRenderer::DEFAULT_VALUE);
                textureRenderer->setPosition(TextureRenderer::RIGHT, TextureRenderer::TOP);
                textureRenderer->initialize("[DEBUG] normal/ambient texture", finalRenderTarget, sceneWidth, sceneHeight);
                debugFramebuffers.emplace_back(std::move(textureRenderer));
            }

            if (DEBUG_DISPLAY_MATERIAL_BUFFER) {
                auto textureRenderer = std::make_unique<TextureRenderer>(materialTexture, TextureRenderer::DEFAULT_VALUE);
                textureRenderer->setPosition(TextureRenderer::LEFT, TextureRenderer::BOTTOM);
                textureRenderer->initialize("[DEBUG] material texture", finalRenderTarget, sceneWidth, sceneHeight);
                debugFramebuffers.emplace_back(std::move(textureRenderer));
            }

            if (DEBUG_DISPLAY_ILLUMINATED_BUFFER) {
                auto textureRenderer = std::make_unique<TextureRenderer>(illuminatedTexture, TextureRenderer::DEFAULT_VALUE);
                textureRenderer->setPosition(TextureRenderer::CENTER_X, TextureRenderer::BOTTOM);
                textureRenderer->initialize("[DEBUG] illuminated texture", finalRenderTarget, sceneWidth, sceneHeight);
                debugFramebuffers.emplace_back(std::move(textureRenderer));
            }

            if (DEBUG_DISPLAY_TRANSPARENT_BUFFER) {
                auto textureRenderer = std::make_unique<TextureRenderer>(transparentManager.getOutputTexture(), TextureRenderer::DEFAULT_VALUE);
                textureRenderer->setPosition(TextureRenderer::CENTER_X, TextureRenderer::BOTTOM);
                textureRenderer->initialize("[DEBUG] transparent texture", finalRenderTarget, sceneWidth, sceneHeight);
                debugFramebuffers.emplace_back(std::move(textureRenderer));
            }

            if (DEBUG_DISPLAY_AMBIENT_OCCLUSION_BUFFER && sceneInfo.isAmbientOcclusionActivated) {
                auto textureRenderer = std::make_unique<TextureRenderer>(ambientOcclusionManager.getAmbientOcclusionTexture(), TextureRenderer::INVERSE_GRAYSCALE_VALUE);
                textureRenderer->setPosition(TextureRenderer::RIGHT, TextureRenderer::BOTTOM);
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

        modelOcclusionCuller.postRefresh();
        lightManager.postUpdateVisibleLights();
    }

}
