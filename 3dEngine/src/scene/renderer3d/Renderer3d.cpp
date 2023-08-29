#include <stdexcept>
#include <locale>

#include <scene/renderer3d/Renderer3d.h>
#include <scene/renderer3d/OpaqueMeshFilter.h>
#include <graphics/render/shader/ShaderBuilder.h>
#include <graphics/render/GenericRendererBuilder.h>
#include <graphics/render/target/NullRenderTarget.h>

namespace urchin {

    //debug parameters
    bool DEBUG_DISPLAY_DEPTH_BUFFER = false;
    bool DEBUG_DISPLAY_ALBEDO_BUFFER = false;
    bool DEBUG_DISPLAY_NORMAL_AMBIENT_BUFFER = false;
    bool DEBUG_DISPLAY_MATERIAL_BUFFER = false;
    bool DEBUG_DISPLAY_ILLUMINATED_BUFFER = false;
    bool DEBUG_DISPLAY_TRANSPARENT_BUFFER = false;
    bool DEBUG_DISPLAY_AMBIENT_OCCLUSION_BUFFER = false;
    bool DEBUG_DISPLAY_MODELS_OCCLUSION_CULLER_DATA = false;
    bool DEBUG_DISPLAY_MODELS_BOUNDING_BOX = false;
    bool DEBUG_DISPLAY_MODEL_BASE_BONES = false;
    bool DEBUG_DISPLAY_LIGHTS_OCTREE = false;

    Renderer3d::Renderer3d(float gammaFactor, RenderTarget& finalRenderTarget, std::shared_ptr<Camera> camera, const VisualConfig& visualConfig, I18nService& i18nService) :
            Renderer(gammaFactor),

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
            modelOcclusionCuller(ModelOcclusionCuller()),
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
            shadowManager(ShadowManager(visualConfig.getShadowConfig(), lightManager, modelOcclusionCuller)),

            //lighting pass rendering
            lightingRenderTarget(finalRenderTarget.isValidRenderTarget() ?
                    std::unique_ptr<RenderTarget>(new OffscreenRender("deferred rendering - second pass", RenderTarget::NO_DEPTH_ATTACHMENT)) :
                    std::unique_ptr<RenderTarget>(new NullRenderTarget(finalRenderTarget.getWidth(), finalRenderTarget.getHeight()))),
            positioningData({}),
            visualOption({}),
            antiAliasingApplier(AntiAliasingApplier(visualConfig.getAntiAliasingConfig(), !finalRenderTarget.isValidRenderTarget())),
            isAntiAliasingActivated(visualConfig.isAntiAliasingActivated()),
            bloomEffectApplier(BloomEffectApplier(visualConfig.getBloomConfig(), finalRenderTarget, getGammaFactor())),

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

    ShadowManager& Renderer3d::getShadowManager() {
        return shadowManager;
    }

    void Renderer3d::activateShadow(bool isShadowActivated) {
        if (visualOption.isShadowActivated != isShadowActivated) {
            visualOption.isShadowActivated = isShadowActivated;

            createOrUpdateLightingPass();
        }
    }

    bool Renderer3d::isShadowActivated() const {
        return visualOption.isShadowActivated;
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

    bool Renderer3d::isAmbientOcclusionActivated() const {
        return visualOption.isAmbientOcclusionActivated;
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

    void Renderer3d::applyUpdatedGammaFactor() {
        bloomEffectApplier.onGammaFactorUpdate(getGammaFactor());
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
        camera->resetPreviousMousePosition();
        paused = false;
    }

    void Renderer3d::prepareRendering(std::uint32_t frameIndex, float dt, unsigned int& screenRenderingOrder) {
        ScopeProfiler sp(Profiler::graphic(), "pre3dRendering");

        updateScene(dt);

        deferredRendering(frameIndex, dt);

        lightingPassRendering(frameIndex);

        unsigned int numDependenciesToTransparentTextures = isAntiAliasingActivated ? 1 /* anti-aliasing */ : 2 /* bloom pre-filter & bloom combine (screen target) */;
        if (DEBUG_DISPLAY_TRANSPARENT_BUFFER) {
            numDependenciesToTransparentTextures += 1;
        }
        transparentManager.drawTransparentModels(frameIndex, numDependenciesToTransparentTextures, *camera);

        if (isAntiAliasingActivated) {
            unsigned int numDependenciesToAATexture = 2; //bloom pre-filter & bloom combine (screen target)
            antiAliasingApplier.applyAntiAliasing(frameIndex, numDependenciesToAATexture);
        }

        bloomEffectApplier.applyBloom(frameIndex, screenRenderingOrder);

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

    void Renderer3d::createOrUpdateLightingPass() {
        //deferred rendering
        albedoTexture = Texture::build("albedo", sceneWidth, sceneHeight, TextureFormat::RGBA_8_INT);
        normalAndAmbientTexture = Texture::build("normal and ambient", sceneWidth, sceneHeight, TextureFormat::RGBA_8_INT);
        materialTexture = Texture::build("material", sceneWidth, sceneHeight, TextureFormat::RG_8_INT);
        if (deferredRenderTarget && deferredRenderTarget->isValidRenderTarget()) {
            auto* deferredOffscreenRenderTarget = static_cast<OffscreenRender*>(deferredRenderTarget.get());
            deferredOffscreenRenderTarget->resetOutputTextures();
            deferredOffscreenRenderTarget->addOutputTexture(albedoTexture);
            deferredOffscreenRenderTarget->addOutputTexture(normalAndAmbientTexture);
            deferredOffscreenRenderTarget->addOutputTexture(materialTexture);
            deferredOffscreenRenderTarget->initialize();
        }

        //lighting pass rendering
        illuminatedTexture = Texture::build("illuminated scene", sceneWidth, sceneHeight, TextureFormat::RGBA_16_FLOAT);
        if (lightingRenderTarget && lightingRenderTarget->isValidRenderTarget()) {
            auto* offscreenLightingRenderTarget = static_cast<OffscreenRender*>(lightingRenderTarget.get());
            offscreenLightingRenderTarget->resetOutputTextures();
            offscreenLightingRenderTarget->addOutputTexture(illuminatedTexture);
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
                ->addUniformTextureReader(TextureReader::build(albedoTexture, TextureParam::buildNearest())) //binding 7
                ->addUniformTextureReader(TextureReader::build(normalAndAmbientTexture, TextureParam::buildNearest())) //binding 8
                ->addUniformTextureReader(TextureReader::build(materialTexture, TextureParam::buildNearest())) //binding 9
                ->addUniformTextureReader(TextureReader::build(Texture::buildEmptyGreyscale("empty AO"), TextureParam::buildNearest())) //binding 10 - ambient occlusion
                ->addUniformTextureReaderArray(shadowMapTextureReaders) //binding 11
                ->build();

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

        //refresh input textures
        if (visualOption.isAmbientOcclusionActivated) {
            ambientOcclusionManager.refreshInputTextures(deferredRenderTarget->getDepthTexture(), normalAndAmbientTexture);
        }
        transparentManager.refreshInputTextures(deferredRenderTarget->getDepthTexture(), illuminatedTexture);
        if (isAntiAliasingActivated) {
            antiAliasingApplier.refreshInputTexture(transparentManager.getOutputTexture());
            bloomEffectApplier.refreshInputTexture(antiAliasingApplier.getOutputTexture());
        } else {
            bloomEffectApplier.refreshInputTexture(transparentManager.getOutputTexture());
        }

        //refresh the model occlusion culler
        modelOcclusionCuller.refresh();

        //determine model visible on scene
        modelsInFrustum.clear();
        modelOcclusionCuller.getModelsInFrustum(camera->getFrustum(), modelsInFrustum);

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
     * Render depth, albedo, normal, etc. into buffers.
     */
    void Renderer3d::deferredRendering(std::uint32_t frameIndex, float dt) {
        ScopeProfiler sp(Profiler::graphic(), "deferredRender");

        //deferred shadow map
        if (visualOption.isShadowActivated) {
            unsigned int numDependenciesToShadowMaps = 1; //second pass
            shadowManager.updateShadowMaps(frameIndex, numDependenciesToShadowMaps);
        }

        //deferred scene (depth, albedo, normal, ambient...)
        deferredRenderTarget->disableAllProcessors();

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
            unsigned int numDependenciesToAOTexture = 1; //second pass
            if (DEBUG_DISPLAY_AMBIENT_OCCLUSION_BUFFER) {
                numDependenciesToAOTexture++;
            }
            ambientOcclusionManager.updateAOTexture(frameIndex, numDependenciesToAOTexture, *camera);
        }
    }

    unsigned int Renderer3d::computeDependenciesToFirstPassOutput() const {
        unsigned int numDependenciesToFirstPassOutput = 1; //second pass
        if (visualOption.isAmbientOcclusionActivated) {
            if (ambientOcclusionManager.getConfig().isBlurActivated) {
                numDependenciesToFirstPassOutput += 3; //AO raw texture & AO vertical filter & AO horizontal filter
            } else {
                numDependenciesToFirstPassOutput += 1; //AO raw texture
            }
        }
        numDependenciesToFirstPassOutput += 1; //transparent
        if (DEBUG_DISPLAY_DEPTH_BUFFER || DEBUG_DISPLAY_ALBEDO_BUFFER || DEBUG_DISPLAY_NORMAL_AMBIENT_BUFFER || DEBUG_DISPLAY_MATERIAL_BUFFER) {
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
     * Compute lighting in pixel shader and render the scene to screen.
     */
    void Renderer3d::lightingPassRendering(std::uint32_t frameIndex) {
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
            constexpr std::size_t ambientOcclusionTexUnit = 4;
            ambientOcclusionManager.loadAOTexture(*lightingRenderer, ambientOcclusionTexUnit);
        }

        if (visualOption.isShadowActivated) {
            constexpr std::size_t shadowMapTexUnit = 5;
            shadowManager.loadShadowMaps(*lightingRenderer, shadowMapTexUnit);
        }

        lightingRenderTarget->render(frameIndex, computeDependenciesToSecondPassOutput());
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
                auto textureRenderer = std::make_unique<TextureRenderer>(deferredRenderTarget->getDepthTexture(), TextureRenderer::GRAYSCALE_VALUE);
                textureRenderer->setPosition(TextureRenderer::LEFT, TextureRenderer::TOP);
                textureRenderer->initialize("[DEBUG] depth texture", finalRenderTarget, sceneWidth, sceneHeight, 0.95f, 1.0f);
                debugFramebuffers.emplace_back(std::move(textureRenderer));
            }

            if (DEBUG_DISPLAY_ALBEDO_BUFFER) {
                auto textureRenderer = std::make_unique<TextureRenderer>(albedoTexture, TextureRenderer::DEFAULT_VALUE);
                textureRenderer->setPosition(TextureRenderer::CENTER_X, TextureRenderer::TOP);
                textureRenderer->initialize("[DEBUG] albedo texture", finalRenderTarget, sceneWidth, sceneHeight);
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

            if (DEBUG_DISPLAY_AMBIENT_OCCLUSION_BUFFER && visualOption.isAmbientOcclusionActivated) {
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
