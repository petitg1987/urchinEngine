#include <stdexcept>
#include <locale>

#include "Renderer3d.h"
#include "graphic/render/GenericRendererBuilder.h"
#include "graphic/shader/builder/ShaderBuilder.h"
#include "graphic/shader/data/ShaderDataSender.h"
#include "texture/render/TextureRenderer.h"
#include "scene/renderer3d/utils/OctreeRenderer.h"

#define DEFAULT_OCTREE_MIN_SIZE 20.0f

namespace urchin {

    //Debug parameters
    bool DEBUG_DISPLAY_DEPTH_BUFFER = false;
    bool DEBUG_DISPLAY_COLOR_BUFFER = false;
    bool DEBUG_DISPLAY_NORMAL_AMBIENT_BUFFER = false;
    bool DEBUG_DISPLAY_ILLUMINATED_SCENE_BUFFER = false;
    bool DEBUG_DISPLAY_SHADOW_MAP = false;
    bool DEBUG_DISPLAY_AMBIENT_OCCLUSION_BUFFER = false;
    bool DEBUG_DISPLAY_MODELS_OCTREE = false;
    bool DEBUG_DISPLAY_MODELS_BOUNDING_BOX = false;
    bool DEBUG_DISPLAY_MODEL_BASE_BONES = false;
    bool DEBUG_DISPLAY_LIGHTS_OCTREE = false;
    bool DEBUG_DISPLAY_LIGHTS_SCENE_BOUNDING_BOX = false;

    Renderer3d::Renderer3d(const RenderTarget* finalRenderTarget) :
            finalRenderTarget(finalRenderTarget),
            sceneWidth(0),
            sceneHeight(0),
            paused(true),
            modelDisplayer(nullptr),
            fogManager(nullptr),
            terrainManager(nullptr),
            waterManager(nullptr),
            skyManager(nullptr),
            geometryManager(nullptr),
            camera(nullptr) {

        //deferred rendering
        deferredRenderTarget = new OffscreenRender();

        modelDisplayer = new ModelDisplayer(ModelDisplayer::DEFAULT_MODE);
        modelDisplayer->initialize();

        modelOctreeManager = new OctreeManager<Model>(DEFAULT_OCTREE_MIN_SIZE);

        fogManager = new FogManager();

        terrainManager = new TerrainManager(deferredRenderTarget);

        waterManager = new WaterManager(deferredRenderTarget);

        skyManager = new SkyManager(deferredRenderTarget);

        geometryManager = new GeometryManager(deferredRenderTarget);

        lightManager = new LightManager(deferredRenderTarget);

        shadowManager = new ShadowManager(lightManager, modelOctreeManager);
        shadowManager->addObserver(this, ShadowManager::NUMBER_SHADOW_MAPS_UPDATE);
        isShadowActivated = true;

        ambientOcclusionManager = new AmbientOcclusionManager();
        isAmbientOcclusionActivated = true;

        //lighting pass rendering
        offscreenLightingRenderTarget = new OffscreenRender();

        createOrUpdateLightingShader();

        antiAliasingManager = new AntiAliasingManager(finalRenderTarget);
        isAntiAliasingActivated = true;
    }

    Renderer3d::~Renderer3d() {
        //models
        for (auto* allOctreeableModel : modelOctreeManager->getAllOctreeables()) {
            delete allOctreeableModel;
        }

        //deferred rendering
        delete deferredRenderTarget;

        delete modelDisplayer;
        delete skyManager;
        delete waterManager;
        delete terrainManager;
        delete fogManager;
        delete geometryManager;
        delete shadowManager;
        delete modelOctreeManager;
        delete lightManager;
        delete ambientOcclusionManager;

        //lighting pass rendering
        delete offscreenLightingRenderTarget;

        delete antiAliasingManager;
    }

    void Renderer3d::createOrUpdateLightingShader() {
        std::locale::global(std::locale("C")); //for float

        std::map<std::string, std::string> tokens;
        tokens["MAX_LIGHTS"] = std::to_string(lightManager->getMaxLights());
        tokens["NUMBER_SHADOW_MAPS"] = std::to_string(shadowManager->getNumberShadowMaps());
        tokens["SHADOW_MAP_BIAS"] = std::to_string(shadowManager->getShadowMapBias());
        tokens["OUTPUT_LOCATION"] = "0"; // isAntiAliasingActivated ? "0" /*TEX_LIGHTING_PASS*/ : "0" /*Screen*/;
        lightingShader = ShaderBuilder().createShader("lighting.vert", "", "lighting.frag", tokens);

        int depthTexUnit = 0;
        int diffuseTexUnit = 1;
        int normalAndAmbientTexUnit = 2;
        ShaderDataSender()
            .sendData(ShaderVar(lightingShader, "depthTex"), depthTexUnit)
            .sendData(ShaderVar(lightingShader, "colorTex"), diffuseTexUnit)
            .sendData(ShaderVar(lightingShader, "normalAndAmbientTex"), normalAndAmbientTexUnit)
            .sendData(ShaderVar(lightingShader, "hasShadow"), isShadowActivated)
            .sendData(ShaderVar(lightingShader, "hasAmbientOcclusion"), isAmbientOcclusionActivated);

        mInverseViewProjectionShaderVar = ShaderVar(lightingShader, "mInverseViewProjection");
        viewPositionShaderVar = ShaderVar(lightingShader, "viewPosition");

        //managers
        fogManager->initiateShaderVariables(lightingShader);
        lightManager->initiateShaderVariables(lightingShader);
        shadowManager->initiateShaderVariables(lightingShader);
        ambientOcclusionManager->initiateShaderVariables(lightingShader);
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

        //deferred rendering
        depthTexture = Texture::build(sceneWidth, sceneHeight, TextureFormat::DEPTH_32_FLOAT, nullptr);
        diffuseTexture = Texture::build(sceneWidth, sceneHeight, TextureFormat::RGBA_8_INT, nullptr);
        normalAndAmbientTexture = Texture::build(sceneWidth, sceneHeight, TextureFormat::RGBA_8_INT, nullptr);
        deferredRenderTarget->onResize(sceneWidth, sceneHeight);
        deferredRenderTarget->removeAllTextures();
        deferredRenderTarget->addTexture(depthTexture);
        deferredRenderTarget->addTexture(diffuseTexture);
        deferredRenderTarget->addTexture(normalAndAmbientTexture);

        //lighting pass rendering
        lightingPassTexture = Texture::build(sceneWidth, sceneHeight, TextureFormat::RGB_8_INT, nullptr);
        offscreenLightingRenderTarget->onResize(sceneWidth, sceneHeight);
        offscreenLightingRenderTarget->removeAllTextures();
        offscreenLightingRenderTarget->addTexture(lightingPassTexture);

        std::vector<Point2<float>> vertexCoord = {
                Point2<float>(-1.0f, 1.0f), Point2<float>(1.0f, 1.0f), Point2<float>(1.0f, -1.0f),
                Point2<float>(-1.0f, 1.0f), Point2<float>(1.0f, -1.0f), Point2<float>(-1.0f, -1.0f)
        };
        std::vector<Point2<float>> textureCoord = {
                Point2<float>(0.0f, 1.0f), Point2<float>(1.0f, 1.0f), Point2<float>(1.0f, 0.0f),
                Point2<float>(0.0f, 1.0f), Point2<float>(1.0f, 0.0f), Point2<float>(0.0f, 0.0f)
        };
        lightingRenderer = std::make_unique<GenericRendererBuilder>(ShapeType::TRIANGLE)
                ->addData(&vertexCoord)
                ->addData(&textureCoord)
                ->addTexture(TextureReader::build(depthTexture, TextureParam::buildNearest()))
                ->addTexture(TextureReader::build(diffuseTexture, TextureParam::buildNearest()))
                ->addTexture(TextureReader::build(normalAndAmbientTexture, TextureParam::buildNearest()))
                ->build();

        shadowManager->onResize(sceneWidth, sceneHeight);

        ambientOcclusionManager->onResize(sceneWidth, sceneHeight);
        ambientOcclusionManager->onTexturesUpdate(depthTexture, normalAndAmbientTexture);

        antiAliasingManager->onResize(sceneWidth, sceneHeight);
        antiAliasingManager->onTextureUpdate(lightingPassTexture);
    }

    void Renderer3d::notify(Observable* observable, int notificationType) {
        if (dynamic_cast<ShadowManager*>(observable)) {
            if (notificationType==ShadowManager::NUMBER_SHADOW_MAPS_UPDATE) {
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
        this->isShadowActivated = isShadowActivated;

        createOrUpdateLightingShader();
        shadowManager->forceUpdateAllShadowMaps();
    }

    AmbientOcclusionManager* Renderer3d::getAmbientOcclusionManager() const {
        return ambientOcclusionManager;
    }

    void Renderer3d::activateAmbientOcclusion(bool isAmbientOcclusionActivated) {
        this->isAmbientOcclusionActivated = isAmbientOcclusionActivated;

        createOrUpdateLightingShader();
    }

    AntiAliasingManager* Renderer3d::getAntiAliasingManager() const {
        return antiAliasingManager;
    }

    void Renderer3d::activateAntiAliasing(bool isAntiAliasingActivated) {
        this->isAntiAliasingActivated = isAntiAliasingActivated;
    }

    void Renderer3d::setCamera(Camera* camera) {
        if (this->camera != nullptr) {
           throw std::runtime_error("Redefine a camera is currently not supported");
        }

        this->camera = camera;
        if (camera) {
            onCameraProjectionUpdate();
        }
    }

    void Renderer3d::onCameraProjectionUpdate() {
        modelDisplayer->onCameraProjectionUpdate(camera);
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
            modelOctreeManager->addOctreeable(model);
        }
    }

    void Renderer3d::removeModel(Model* model) {
        if (model) {
            modelOctreeManager->removeOctreeable(model);
        }
        delete model;
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
            camera->resetMousePosition();
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

    bool Renderer3d::onChar(unsigned int) {
        //nothing to do
        return true;
    }

    bool Renderer3d::onMouseMove(int mouseX, int mouseY) {
        if (!paused && camera) {
            return camera->onMouseMove(mouseX, mouseY);
        }
        return true;
    }

    void Renderer3d::onDisable() {
        //nothing to do
    }

    void Renderer3d::display(float dt) {
        ScopeProfiler profiler("3d", "rendererDisplay");

        if (!camera) { //nothing to display if camera doesn't exist
            return;
        }

        updateScene(dt);

        deferredRendering(dt);
        lightingPassRendering();

        if(isAntiAliasingActivated) {
            antiAliasingManager->applyAntiAliasing();
        }

        postUpdateScene();

        displayBuffers();
    }

    void Renderer3d::displayBuffers() {
        if (DEBUG_DISPLAY_DEPTH_BUFFER) {
            float depthIntensity = 5.0f;
            TextureRenderer textureRenderer(depthTexture, TextureRenderer::DEPTH_VALUE, depthIntensity);
            textureRenderer.setPosition(TextureRenderer::LEFT, TextureRenderer::TOP);
            textureRenderer.initialize(sceneWidth, sceneHeight, camera->getNearPlane(), camera->getFarPlane());
            textureRenderer.display(finalRenderTarget);
        }

        if (DEBUG_DISPLAY_COLOR_BUFFER) {
            TextureRenderer textureRenderer(diffuseTexture, TextureRenderer::DEFAULT_VALUE);
            textureRenderer.setPosition(TextureRenderer::CENTER_X, TextureRenderer::TOP);
            textureRenderer.initialize(sceneWidth, sceneHeight, camera->getNearPlane(), camera->getFarPlane());
            textureRenderer.display(finalRenderTarget);
        }

        if (DEBUG_DISPLAY_NORMAL_AMBIENT_BUFFER) {
            TextureRenderer textureRenderer(normalAndAmbientTexture, TextureRenderer::DEFAULT_VALUE);
            textureRenderer.setPosition(TextureRenderer::RIGHT, TextureRenderer::TOP);
            textureRenderer.initialize(sceneWidth, sceneHeight, camera->getNearPlane(), camera->getFarPlane());
            textureRenderer.display(finalRenderTarget);
        }

        if (DEBUG_DISPLAY_ILLUMINATED_SCENE_BUFFER) {
            TextureRenderer textureRenderer(lightingPassTexture, TextureRenderer::DEFAULT_VALUE);
            textureRenderer.setPosition(TextureRenderer::LEFT, TextureRenderer::BOTTOM);
            textureRenderer.initialize(sceneWidth, sceneHeight, camera->getNearPlane(), camera->getFarPlane());
            textureRenderer.display(finalRenderTarget);
        }

        if (DEBUG_DISPLAY_SHADOW_MAP) {
            const Light* firstLight = lightManager->getVisibleLights()[0]; //choose light
            const unsigned int shadowMapNumber = 0; //choose shadow map to display [0, nbShadowMaps-1]
            TextureRenderer textureDisplayer(shadowManager->getShadowData(firstLight).getShadowMapTexture(), shadowMapNumber, TextureRenderer::DEFAULT_VALUE);
            textureDisplayer.setPosition(TextureRenderer::CENTER_X, TextureRenderer::BOTTOM);
            textureDisplayer.initialize(sceneWidth, sceneHeight, camera->getNearPlane(), camera->getFarPlane());
            textureDisplayer.display(finalRenderTarget);
        }

        if (DEBUG_DISPLAY_AMBIENT_OCCLUSION_BUFFER) {
            float ambientOcclusionIntensity = 10.0f;
            TextureRenderer textureRenderer(ambientOcclusionManager->getAmbientOcclusionTexture(), TextureRenderer::INVERSE_GRAYSCALE_VALUE, ambientOcclusionIntensity);
            textureRenderer.setPosition(TextureRenderer::RIGHT, TextureRenderer::BOTTOM);
            textureRenderer.initialize(sceneWidth, sceneHeight, camera->getNearPlane(), camera->getFarPlane());
            textureRenderer.display(finalRenderTarget);
        }
    }

    void Renderer3d::updateScene(float dt) {
        ScopeProfiler profiler("3d", "updateScene");

        //move the camera
        camera->updateCameraView(dt);

        //refresh models in octree
        modelOctreeManager->refreshOctreeables();

        //determine visible lights on scene
        lightManager->updateLights(camera->getFrustum());

        //determine models producing shadow on scene
        if (isShadowActivated) {
            shadowManager->updateVisibleModels(camera->getFrustum());
        }

        //animate models (only those visible to scene OR producing shadow on scene)
        if (isShadowActivated) {
            modelDisplayer->setModels(shadowManager->computeVisibleModels());
        } else {
            updateModelsInFrustum();
            modelDisplayer->setModels(modelsInFrustum);
        }
        modelDisplayer->updateAnimation(dt);

        //update shadow maps
        if (isShadowActivated) {
            shadowManager->updateShadowMaps();
        }
    }

    /**
     * First pass of deferred shading algorithm.
     * Render depth, color, normal, etc. into buffers.
     */
    void Renderer3d::deferredRendering(float dt) {
        ScopeProfiler profiler("3d", "deferredRender");

        deferredRenderTarget->resetDisplay();

        skyManager->display(camera->getViewMatrix(), camera->getPosition());

        updateModelsInFrustum();
        modelDisplayer->setModels(modelsInFrustum);
        modelDisplayer->setRenderTarget(deferredRenderTarget);
        modelDisplayer->display(camera->getViewMatrix());

        terrainManager->display(camera, dt);

        waterManager->display(camera, fogManager, dt);

        geometryManager->display(camera->getViewMatrix());

        if (isAmbientOcclusionActivated) {
            ambientOcclusionManager->updateAOTexture(camera);
        }

        displayDetails();
    }

    void Renderer3d::displayDetails() {
        if (DEBUG_DISPLAY_MODELS_OCTREE) {
            OctreeRenderer::drawOctree(modelOctreeManager, deferredRenderTarget, camera->getProjectionMatrix(), camera->getViewMatrix());
        }

        if (DEBUG_DISPLAY_MODELS_BOUNDING_BOX) {
            modelDisplayer->drawBBox(camera->getProjectionMatrix(), camera->getViewMatrix());
        }

        if (DEBUG_DISPLAY_MODEL_BASE_BONES) {
            modelDisplayer->drawBaseBones(camera->getProjectionMatrix(), camera->getViewMatrix(), "models/character.urchinMesh");
        }

        if (DEBUG_DISPLAY_LIGHTS_OCTREE) {
            lightManager->drawLightOctree(camera->getProjectionMatrix(), camera->getViewMatrix());
        }

        if (DEBUG_DISPLAY_LIGHTS_SCENE_BOUNDING_BOX) { //display scene box visible from light based on split frustums
            const Light* firstLight = lightManager->getVisibleLights()[0]; //choose light
            for (const auto& frustum : shadowManager->getSplitFrustums()) {
                shadowManager->drawLightSceneBox(deferredRenderTarget, frustum, firstLight, camera->getViewMatrix());
            }
        }
    }

    /**
     * Second pass of deferred shading algorithm.
     * Compute lighting in pixel shader and render the scene to screen.
     */
    void Renderer3d::lightingPassRendering() {
        if(lightingRenderer) {
            ScopeProfiler profiler("3d", "lightPassRender");

            lightingRenderer->clearAdditionalTextures();
            ShaderDataSender()
                    .sendData(mInverseViewProjectionShaderVar, (camera->getProjectionMatrix() * camera->getViewMatrix()).inverse())
                    .sendData(viewPositionShaderVar, camera->getPosition());

            lightManager->loadLights();

            fogManager->loadFog();

            if (isAmbientOcclusionActivated) {
                ambientOcclusionManager->loadAOTexture(lightingRenderer);
            }

            if (isShadowActivated) {
                shadowManager->loadShadowMaps(lightingRenderer);
            }

            auto* lightingRenderTarget = isAntiAliasingActivated ? offscreenLightingRenderTarget : finalRenderTarget;
            lightingRenderTarget->activeShader(lightingShader);
            lightingRenderTarget->display(lightingRenderer);
        }
    }

    void Renderer3d::postUpdateScene() {
        ScopeProfiler profiler("3d", "postUpdateScene");

        modelOctreeManager->postRefreshOctreeables();

        lightManager->postUpdateLights();
    }

}
