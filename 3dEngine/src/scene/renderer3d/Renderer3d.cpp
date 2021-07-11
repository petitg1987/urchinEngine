#include <stdexcept>
#include <locale>

#include <scene/renderer3d/Renderer3d.h>
#include <graphic/render/GenericRendererBuilder.h>
#include <graphic/render/shader/builder/ShaderBuilder.h>
#include <scene/renderer3d/util/OctreeRenderer.h>

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
            finalRenderTarget(finalRenderTarget),
            sceneWidth(finalRenderTarget.getWidth()),
            sceneHeight(finalRenderTarget.getHeight()),
            paused(true),
            camera(nullptr),

            //deferred rendering
            deferredRenderTarget(std::make_unique<OffscreenRender>("deferred rendering - first pass", RenderTarget::READ_WRITE_DEPTH_ATTACHMENT)),
            modelSetDisplayer(std::make_unique<ModelSetDisplayer>(DisplayMode::DEFAULT_MODE)),
            modelOctreeManager(new OctreeManager<Model>(20.0f)),
            fogManager(new FogManager()),
            terrainManager(new TerrainManager(*deferredRenderTarget)),
            waterManager(new WaterManager(*deferredRenderTarget)),
            skyManager(new SkyManager(*deferredRenderTarget)),
            geometryManager(new GeometryManager(*deferredRenderTarget)),
            lightManager(new LightManager(*deferredRenderTarget)),
            ambientOcclusionManager(new AmbientOcclusionManager()),
            shadowManager(new ShadowManager(lightManager, modelOctreeManager)),

            //lighting pass rendering
            offscreenLightingRenderTarget(std::make_unique<OffscreenRender>("deferred rendering - second pass", RenderTarget::NO_DEPTH_ATTACHMENT)),
            positioningData({}),
            visualOption({}),
            antiAliasingManager(new AntiAliasingManager(finalRenderTarget)),
            isAntiAliasingActivated(true) {

        //deferred rendering
        modelSetDisplayer->initialize(*deferredRenderTarget);
        shadowManager->addObserver(this, ShadowManager::NUMBER_SHADOW_MAPS_UPDATE);

        //lighting pass rendering
        visualOption.isShadowActivated = true;
        visualOption.isAmbientOcclusionActivated = true;
        createOrUpdateLightingShader();
    }

    Renderer3d::~Renderer3d() {
        //models
        modelSetDisplayer.reset(nullptr);
        for (auto* allOctreeableModel : modelOctreeManager->getAllOctreeables()) {
            delete allOctreeableModel;
        }

        offscreenLightingRenderTarget->cleanup();
        deferredRenderTarget->cleanup();
        delete skyManager;
        delete waterManager;
        delete terrainManager;
        delete fogManager;
        delete geometryManager;
        delete shadowManager;
        delete modelOctreeManager;
        delete lightManager;
        delete ambientOcclusionManager;

        delete antiAliasingManager;
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

        refreshRenderer();
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

        refreshRenderer();
    }

    void Renderer3d::notify(Observable* observable, int notificationType) {
        if (dynamic_cast<ShadowManager*>(observable)) {
            if (notificationType == ShadowManager::NUMBER_SHADOW_MAPS_UPDATE) {
                createOrUpdateLightingShader();
            }
        }
    }

    OctreeManager<Model>* Renderer3d::getModelOctreeManager() const {
        return modelOctreeManager;
    }

    FogManager* Renderer3d::getFogManager() const {
        return fogManager;
    }

    TerrainManager* Renderer3d::getTerrainManager() const {
        return terrainManager;
    }

    WaterManager* Renderer3d::getWaterManager() const {
        return waterManager;
    }

    SkyManager* Renderer3d::getSkyManager() const {
        return skyManager;
    }

    GeometryManager* Renderer3d::getGeometryManager() const {
        return geometryManager;
    }

    LightManager* Renderer3d::getLightManager() const {
        return lightManager;
    }

    ShadowManager* Renderer3d::getShadowManager() const {
        return shadowManager;
    }

    void Renderer3d::activateShadow(bool isShadowActivated) {
        if (visualOption.isShadowActivated != isShadowActivated) {
            visualOption.isShadowActivated = isShadowActivated;

            createOrUpdateLightingShader();
            shadowManager->forceUpdateAllShadowMaps();
        }
    }

    AmbientOcclusionManager* Renderer3d::getAmbientOcclusionManager() const {
        return ambientOcclusionManager;
    }

    void Renderer3d::activateAmbientOcclusion(bool isAmbientOcclusionActivated) {
        if (visualOption.isAmbientOcclusionActivated != isAmbientOcclusionActivated) {
            visualOption.isAmbientOcclusionActivated = isAmbientOcclusionActivated;

            createOrUpdateLightingShader();
        }
    }

    AntiAliasingManager* Renderer3d::getAntiAliasingManager() const {
        return antiAliasingManager;
    }

    void Renderer3d::activateAntiAliasing(bool isAntiAliasingActivated) {
        if (this->isAntiAliasingActivated != isAntiAliasingActivated) {
            this->isAntiAliasingActivated = isAntiAliasingActivated;
            refreshRenderer();
        }
    }

    void Renderer3d::setCamera(Camera* camera) {
        if (this->camera != nullptr) {
           throw std::runtime_error("Redefine a camera is currently not supported");
        }

        this->camera = camera;
        if (camera) {
            camera->initialize(sceneWidth, sceneHeight);
            onCameraProjectionUpdate();
        }
    }

    void Renderer3d::onCameraProjectionUpdate() {
        modelSetDisplayer->onCameraProjectionUpdate(camera);
        terrainManager->onCameraProjectionUpdate(camera);
        waterManager->onCameraProjectionUpdate(camera);
        skyManager->onCameraProjectionUpdate(camera);
        geometryManager->onCameraProjectionUpdate(camera);
        shadowManager->onCameraProjectionUpdate(camera);
        ambientOcclusionManager->onCameraProjectionUpdate(camera);
    }

    void Renderer3d::updateModelsInFrustum() {
        modelsInFrustum.clear();
        modelOctreeManager->getOctreeablesIn(getCamera()->getFrustum(), modelsInFrustum);
    }

    Camera* Renderer3d::getCamera() const {
        return camera;
    }

    void Renderer3d::addModel(Model* model) {
        if (model) {
            Logger::instance()->logInfo("Add 3D model: " + model->getConstMeshes()->getName());
            modelOctreeManager->addOctreeable(model);
        }
    }

    void Renderer3d::removeModel(Model* model) {
        if (model) {
            shadowManager->removeModel(model);
            modelSetDisplayer->removeModel(model);

            modelOctreeManager->removeOctreeable(model);
            delete model;
        }
    }

    bool Renderer3d::isModelExist(Model* model) {
        std::vector<Model*> allOctreeables = modelOctreeManager->getAllOctreeables();
        return std::find(allOctreeables.begin(), allOctreeables.end(), model) != allOctreeables.end();
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

    void Renderer3d::refreshRenderer() {
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
        lightManager->setupLightingRenderer(lightingRendererBuilder); //binding 2 & 3
        shadowManager->setupLightingRenderer(lightingRendererBuilder); //binding 4 & 5
        fogManager->setupLightingRenderer(lightingRendererBuilder); //binding 6

        std::vector<std::shared_ptr<TextureReader>> shadowMapTextureReaders;
        for (unsigned int i = 0; i < shadowManager->getMaxShadowLights(); ++i) {
            shadowMapTextureReaders.push_back(TextureReader::build(Texture::buildEmptyArrayRg(), TextureParam::buildNearest()));
        }
        lightingRenderer = lightingRendererBuilder
                ->addUniformTextureReader(TextureReader::build(deferredRenderTarget->getDepthTexture(), TextureParam::buildNearest())) //binding 7
                ->addUniformTextureReader(TextureReader::build(diffuseTexture, TextureParam::buildNearest())) //binding 8
                ->addUniformTextureReader(TextureReader::build(normalAndAmbientTexture, TextureParam::buildNearest())) //binding 9
                ->addUniformTextureReader(TextureReader::build(Texture::buildEmptyRgba(), TextureParam::buildNearest())) //binding 10 - ambient occlusion
                ->addUniformTextureReaderArray(shadowMapTextureReaders) //binding 11
                ->build();

        ambientOcclusionManager->onResize(sceneWidth, sceneHeight);
        ambientOcclusionManager->onTexturesUpdate(deferredRenderTarget->getDepthTexture(), normalAndAmbientTexture);

        if(isAntiAliasingActivated) {
            antiAliasingManager->onResize(sceneWidth, sceneHeight);
            antiAliasingManager->onTextureUpdate(lightingPassTexture);
        }

        setupDebugFramebuffers();
    }

    void Renderer3d::setupDebugFramebuffers() {
        debugFramebuffers.clear();

        if(camera) {
            if (DEBUG_DISPLAY_DEPTH_BUFFER) {
                float depthIntensity = 5.0f;
                auto textureRenderer = std::make_unique<TextureRenderer>(deferredRenderTarget->getDepthTexture(), TextureRenderer::DEPTH_VALUE, depthIntensity);
                textureRenderer->setPosition(TextureRenderer::LEFT, TextureRenderer::TOP);
                textureRenderer->initialize("[DEBUG] depth texture", finalRenderTarget, sceneWidth, sceneHeight, camera->getNearPlane(), camera->getFarPlane());
                debugFramebuffers.emplace_back(std::move(textureRenderer));
            }

            if (DEBUG_DISPLAY_COLOR_BUFFER) {
                auto textureRenderer = std::make_unique<TextureRenderer>(diffuseTexture, TextureRenderer::DEFAULT_VALUE);
                textureRenderer->setPosition(TextureRenderer::CENTER_X, TextureRenderer::TOP);
                textureRenderer->initialize("[DEBUG] diffuse texture", finalRenderTarget, sceneWidth, sceneHeight, camera->getNearPlane(), camera->getFarPlane());
                debugFramebuffers.emplace_back(std::move(textureRenderer));
            }

            if (DEBUG_DISPLAY_NORMAL_AMBIENT_BUFFER) {
                auto textureRenderer = std::make_unique<TextureRenderer>(normalAndAmbientTexture, TextureRenderer::DEFAULT_VALUE);
                textureRenderer->setPosition(TextureRenderer::RIGHT, TextureRenderer::TOP);
                textureRenderer->initialize("[DEBUG] normal/ambient texture", finalRenderTarget, sceneWidth, sceneHeight, camera->getNearPlane(), camera->getFarPlane());
                debugFramebuffers.emplace_back(std::move(textureRenderer));
            }

            if (DEBUG_DISPLAY_ILLUMINATED_SCENE_BUFFER) {
                auto textureRenderer = std::make_unique<TextureRenderer>(lightingPassTexture, TextureRenderer::DEFAULT_VALUE);
                textureRenderer->setPosition(TextureRenderer::LEFT, TextureRenderer::BOTTOM);
                textureRenderer->initialize("[DEBUG] lighting pass texture", finalRenderTarget, sceneWidth, sceneHeight, camera->getNearPlane(), camera->getFarPlane());
                debugFramebuffers.emplace_back(std::move(textureRenderer));
            }

            if (DEBUG_DISPLAY_SHADOW_MAP_BUFFER) {
                const Light *firstLight = lightManager->getSunLights()[0]; //choose light
                const unsigned int shadowMapNumber = 0; //choose shadow map to display [0, nbShadowMaps - 1]
                auto textureRenderer = std::make_unique<TextureRenderer>(shadowManager->getLightShadowMap(firstLight).getShadowMapTexture(), shadowMapNumber, TextureRenderer::DEFAULT_VALUE);
                textureRenderer->setPosition(TextureRenderer::CENTER_X, TextureRenderer::BOTTOM);
                textureRenderer->initialize("[DEBUG] shadow map", finalRenderTarget, sceneWidth, sceneHeight, camera->getNearPlane(), camera->getFarPlane());
                debugFramebuffers.emplace_back(std::move(textureRenderer));
            }

            if (DEBUG_DISPLAY_AMBIENT_OCCLUSION_BUFFER) {
                float ambientOcclusionIntensity = 10.0f;
                auto textureRenderer = std::make_unique<TextureRenderer>(ambientOcclusionManager->getAmbientOcclusionTexture(), TextureRenderer::INVERSE_GRAYSCALE_VALUE, ambientOcclusionIntensity);
                textureRenderer->setPosition(TextureRenderer::RIGHT, TextureRenderer::BOTTOM);
                textureRenderer->initialize("[DEBUG] ambient occlusion texture", finalRenderTarget, sceneWidth, sceneHeight, camera->getNearPlane(), camera->getFarPlane());
                debugFramebuffers.emplace_back(std::move(textureRenderer));
            }
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
        skyManager->prepareRendering(camera->getViewMatrix(), camera->getPosition());
        modelSetDisplayer->prepareRendering(camera->getViewMatrix());
        terrainManager->prepareRendering(camera, dt);
        waterManager->prepareRendering(camera, fogManager, dt);
        geometryManager->prepareRendering(camera->getViewMatrix());
        renderDebugSceneData();
        deferredRenderTarget->render();

        //deferred ambient occlusion
        if (visualOption.isAmbientOcclusionActivated) {
            ambientOcclusionManager->updateAOTexture(camera);
        }
    }

    void Renderer3d::renderDebugSceneData() {
        if (DEBUG_DISPLAY_MODELS_OCTREE) {
            debugModelOctree = OctreeRenderer::createOctreeModel(modelOctreeManager, *deferredRenderTarget, camera->getProjectionMatrix());
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
        lightingRenderer->updateUniformData(0, &positioningData);

        lightManager->loadVisibleLights(lightingRenderer);

        fogManager->loadFog(lightingRenderer);

        if (visualOption.isAmbientOcclusionActivated) {
            std::size_t ambientOcclusionTexUnit = 3;
            ambientOcclusionManager->loadAOTexture(lightingRenderer, ambientOcclusionTexUnit);
        }

        if (visualOption.isShadowActivated) {
            std::size_t shadowMapTexUnit = 4;
            shadowManager->loadShadowMaps(lightingRenderer, shadowMapTexUnit);
        }

        if (isAntiAliasingActivated) {
            offscreenLightingRenderTarget->render();
        } else {
            lightingRenderer->enableRenderer();
        }
    }

    void Renderer3d::renderDebugFramebuffers() {
        for(auto& debugFramebuffer : debugFramebuffers) {
            debugFramebuffer->prepareRendering();
        }
    }

    void Renderer3d::postUpdateScene() {
        ScopeProfiler sp(Profiler::graphic(), "postUpdateScene");

        modelOctreeManager->postRefreshOctreeables();
        lightManager->postUpdateVisibleLights();
    }

}
