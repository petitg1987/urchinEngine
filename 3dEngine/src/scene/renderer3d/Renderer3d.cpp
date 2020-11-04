#include <GL/glew.h>
#include <stdexcept>
#include <locale>

#include "Renderer3d.h"
#include "graphic/displayer/generic/GenericDisplayerBuilder.h"
#include "graphic/displayer/texture/TextureDisplayer.h"
#include "graphic/shader/ShaderManager.h"
#include "scene/renderer3d/utils/OctreeDisplayer.h"

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

    Renderer3d::Renderer3d() :
            sceneWidth(0),
            sceneHeight(0),
            paused(true),
            modelDisplayer(nullptr),
            fogManager(nullptr),
            terrainManager(nullptr),
            waterManager(nullptr),
            skyManager(nullptr),
            geometryManager(nullptr),
            camera(nullptr),
            fboIDs(nullptr),
            fboAttachments{GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2},
            textureIDs(nullptr),
            deferredShadingShader(0),
            mInverseViewProjectionLoc(0),
            viewPositionLoc(0) {
        //deferred shading (pass 1)
        fboIDs = new unsigned int[1];
        textureIDs = new unsigned int[4];
        modelDisplayer = new ModelDisplayer(ModelDisplayer::DEFAULT_MODE);
        modelDisplayer->initialize();
        glGenFramebuffers(1, fboIDs);
        glGenTextures(4, textureIDs);

        modelOctreeManager = new OctreeManager<Model>(DEFAULT_OCTREE_MIN_SIZE);

        fogManager = new FogManager();

        terrainManager = new TerrainManager();

        waterManager = new WaterManager();

        skyManager = new SkyManager();

        geometryManager = new GeometryManager();

        lightManager = new LightManager();

        shadowManager = new ShadowManager(lightManager, modelOctreeManager);
        shadowManager->addObserver(this, ShadowManager::NUMBER_SHADOW_MAPS_UPDATE);
        isShadowActivated = true;

        ambientOcclusionManager = new AmbientOcclusionManager(textureIDs[TEX_DEPTH], textureIDs[TEX_NORMAL_AND_AMBIENT]);
        isAmbientOcclusionActivated = true;

        //deferred shading (pass 2)
        createOrUpdateDeferredShadingShader();
        lightingPassDisplayer = std::make_unique<GenericDisplayerBuilder>()->build();

        antiAliasingManager = new AntiAliasingManager();
        isAntiAliasingActivated = true;
    }

    Renderer3d::~Renderer3d() {
        //models
        for (auto *allOctreeableModel : modelOctreeManager->getAllOctreeables()) {
            delete allOctreeableModel;
        }

        //managers
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
        delete antiAliasingManager;

        //deferred shading (pass 1)
        if (fboIDs) {
            glDeleteFramebuffers(1, fboIDs);
            delete [] fboIDs;
        }
        if (textureIDs) {
            glDeleteTextures(4, textureIDs);
            delete [] textureIDs;
        }

        //deferred shading (pass 2)
        ShaderManager::instance()->removeProgram(deferredShadingShader);
    }

    void Renderer3d::createOrUpdateDeferredShadingShader() {
        std::locale::global(std::locale("C")); //for float

        std::map<std::string, std::string> tokens;
        tokens["MAX_LIGHTS"] = std::to_string(lightManager->getMaxLights());
        tokens["NUMBER_SHADOW_MAPS"] = std::to_string(shadowManager->getNumberShadowMaps());
        tokens["SHADOW_MAP_BIAS"] = std::to_string(shadowManager->getShadowMapBias());
        tokens["OUTPUT_LOCATION"] = "0"; // isAntiAliasingActivated ? "0" /*TEX_LIGHTING_PASS*/ : "0" /*Screen*/;
        ShaderManager::instance()->removeProgram(deferredShadingShader);
        deferredShadingShader = ShaderManager::instance()->createProgram("deferredShading.vert", "", "deferredShading.frag", tokens);
        ShaderManager::instance()->bind(deferredShadingShader);

        int depthTexLoc = glGetUniformLocation(deferredShadingShader, "depthTex");
        glUniform1i(depthTexLoc, GL_TEXTURE0-GL_TEXTURE0);
        int diffuseTexLoc = glGetUniformLocation(deferredShadingShader, "colorTex");
        glUniform1i(diffuseTexLoc, GL_TEXTURE1-GL_TEXTURE0);
        int normalAndAmbientTexLoc = glGetUniformLocation(deferredShadingShader, "normalAndAmbientTex");
        glUniform1i(normalAndAmbientTexLoc, GL_TEXTURE2-GL_TEXTURE0);
        int hasShadowLoc = glGetUniformLocation(deferredShadingShader, "hasShadow");
        glUniform1i(hasShadowLoc, isShadowActivated);
        int hasAmbientOcclusionLoc = glGetUniformLocation(deferredShadingShader, "hasAmbientOcclusion");
        glUniform1i(hasAmbientOcclusionLoc, isAmbientOcclusionActivated);
        mInverseViewProjectionLoc = glGetUniformLocation(deferredShadingShader, "mInverseViewProjection");
        viewPositionLoc = glGetUniformLocation(deferredShadingShader, "viewPosition");

        //managers
        fogManager->loadUniformLocationFor(deferredShadingShader);
        lightManager->loadUniformLocationFor(deferredShadingShader);
        shadowManager->loadUniformLocationFor(deferredShadingShader);
        ambientOcclusionManager->loadUniformLocationFor(deferredShadingShader);
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

        //deferred shading
        glBindFramebuffer(GL_FRAMEBUFFER, fboIDs[FBO_SCENE]);

        glBindTexture(GL_TEXTURE_2D, textureIDs[TEX_DEPTH]); //depth buffer
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, sceneWidth, sceneHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureIDs[TEX_DEPTH], 0);

        glBindTexture(GL_TEXTURE_2D, textureIDs[TEX_DIFFUSE]); //diffuse buffer
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sceneWidth, sceneHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glFramebufferTexture2D(GL_FRAMEBUFFER, fboAttachments[0], GL_TEXTURE_2D, textureIDs[TEX_DIFFUSE], 0);

        glBindTexture(GL_TEXTURE_2D, textureIDs[TEX_NORMAL_AND_AMBIENT]); //normal and ambient factor buffer
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sceneWidth, sceneHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glFramebufferTexture2D(GL_FRAMEBUFFER, fboAttachments[1], GL_TEXTURE_2D, textureIDs[TEX_NORMAL_AND_AMBIENT], 0);

        glBindTexture(GL_TEXTURE_2D, textureIDs[TEX_LIGHTING_PASS]); //illuminated scene buffer
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sceneWidth, sceneHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glFramebufferTexture2D(GL_FRAMEBUFFER, fboAttachments[2], GL_TEXTURE_2D, textureIDs[TEX_LIGHTING_PASS], 0);

        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //manager
        shadowManager->onResize(sceneWidth, sceneHeight);
        ambientOcclusionManager->onResize(sceneWidth, sceneHeight);
        antiAliasingManager->onResize(sceneWidth, sceneHeight);
    }

    void Renderer3d::notify(Observable *observable, int notificationType) {
        if (dynamic_cast<ShadowManager *>(observable)) {
            if (notificationType==ShadowManager::NUMBER_SHADOW_MAPS_UPDATE) {
                createOrUpdateDeferredShadingShader();
            }
        }
    }

    OctreeManager<Model> *Renderer3d::getModelOctreeManager() const {
        return modelOctreeManager;
    }

    FogManager *Renderer3d::getFogManager() const {
        return fogManager;
    }

    TerrainManager *Renderer3d::getTerrainManager() const {
        return terrainManager;
    }

    WaterManager *Renderer3d::getWaterManager() const {
        return waterManager;
    }

    SkyManager *Renderer3d::getSkyManager() const {
        return skyManager;
    }

    GeometryManager *Renderer3d::getGeometryManager() const {
        return geometryManager;
    }

    LightManager *Renderer3d::getLightManager() const {
        return lightManager;
    }

    ShadowManager *Renderer3d::getShadowManager() const {
        return shadowManager;
    }

    void Renderer3d::activateShadow(bool isShadowActivated) {
        this->isShadowActivated = isShadowActivated;

        createOrUpdateDeferredShadingShader();
        shadowManager->forceUpdateAllShadowMaps();
    }

    AmbientOcclusionManager *Renderer3d::getAmbientOcclusionManager() const {
        return ambientOcclusionManager;
    }

    void Renderer3d::activateAmbientOcclusion(bool isAmbientOcclusionActivated) {
        this->isAmbientOcclusionActivated = isAmbientOcclusionActivated;

        createOrUpdateDeferredShadingShader();
    }

    AntiAliasingManager *Renderer3d::getAntiAliasingManager() const {
        return antiAliasingManager;
    }

    void Renderer3d::activateAntiAliasing(bool isAntiAliasingActivated) {
        this->isAntiAliasingActivated = isAntiAliasingActivated;
    }

    void Renderer3d::setCamera(Camera *camera) {
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

    Camera *Renderer3d::getCamera() const {
        return camera;
    }

    void Renderer3d::addModel(Model *model) {
        if (model) {
            modelOctreeManager->addOctreeable(model);
        }
    }

    void Renderer3d::removeModel(Model *model) {
        if (model) {
            modelOctreeManager->removeOctreeable(model);
        }
        delete model;
    }

    bool Renderer3d::isModelExist(Model *model) {
        std::vector<Model *> allOctreeables = modelOctreeManager->getAllOctreeables();
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

        glBindFramebuffer(GL_FRAMEBUFFER, fboIDs[FBO_SCENE]);
        glDrawBuffers(2, &fboAttachments[0]);
        deferredGeometryRendering(dt);

        if (isAntiAliasingActivated) {
            glDrawBuffers(1, &fboAttachments[2]);
            lightingPassRendering();

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            antiAliasingManager->applyOn(textureIDs[TEX_LIGHTING_PASS]);
        } else {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            lightingPassRendering();
        }

        postUpdateScene();
        displayBuffers();
    }

    void Renderer3d::displayBuffers() {
        if (DEBUG_DISPLAY_DEPTH_BUFFER) {
            float depthIntensity = 5.0f;
            TextureDisplayer textureDisplayer(textureIDs[TEX_DEPTH], TextureDisplayer::DEPTH_VALUE, depthIntensity);
            textureDisplayer.setPosition(TextureDisplayer::LEFT, TextureDisplayer::TOP);
            textureDisplayer.initialize(sceneWidth, sceneHeight, camera->getNearPlane(), camera->getFarPlane());
            textureDisplayer.display();
        }

        if (DEBUG_DISPLAY_COLOR_BUFFER) {
            TextureDisplayer textureDisplayer(textureIDs[TEX_DIFFUSE], TextureDisplayer::DEFAULT_VALUE);
            textureDisplayer.setPosition(TextureDisplayer::CENTER_X, TextureDisplayer::TOP);
            textureDisplayer.initialize(sceneWidth, sceneHeight, camera->getNearPlane(), camera->getFarPlane());
            textureDisplayer.display();
        }

        if (DEBUG_DISPLAY_NORMAL_AMBIENT_BUFFER) {
            TextureDisplayer textureDisplayer(textureIDs[TEX_NORMAL_AND_AMBIENT], TextureDisplayer::DEFAULT_VALUE);
            textureDisplayer.setPosition(TextureDisplayer::RIGHT, TextureDisplayer::TOP);
            textureDisplayer.initialize(sceneWidth, sceneHeight, camera->getNearPlane(), camera->getFarPlane());
            textureDisplayer.display();
        }

        if (DEBUG_DISPLAY_ILLUMINATED_SCENE_BUFFER) {
            TextureDisplayer textureDisplayer(textureIDs[TEX_LIGHTING_PASS], TextureDisplayer::DEFAULT_VALUE);
            textureDisplayer.setPosition(TextureDisplayer::LEFT, TextureDisplayer::BOTTOM);
            textureDisplayer.initialize(sceneWidth, sceneHeight, camera->getNearPlane(), camera->getFarPlane());
            textureDisplayer.display();
        }

        if (DEBUG_DISPLAY_SHADOW_MAP) {
            const Light *firstLight = lightManager->getVisibleLights()[0]; //choose light
            const unsigned int shadowMapNumber = 0; //choose shadow map to display [0, nbShadowMaps-1]
            unsigned int shadowMapTextureID = shadowManager->getShadowData(firstLight).getShadowMapTextureID();
            TextureDisplayer textureDisplayer(shadowMapTextureID, shadowMapNumber, TextureDisplayer::DEFAULT_VALUE);
            textureDisplayer.setPosition(TextureDisplayer::CENTER_X, TextureDisplayer::BOTTOM);
            textureDisplayer.initialize(sceneWidth, sceneHeight, camera->getNearPlane(), camera->getFarPlane());
            textureDisplayer.display();
        }

        if (DEBUG_DISPLAY_AMBIENT_OCCLUSION_BUFFER) {
            float ambientOcclusionIntensity = 10.0f;
            TextureDisplayer textureDisplayer(ambientOcclusionManager->getAmbientOcclusionTextureID(), TextureDisplayer::INVERSE_GRAYSCALE_VALUE, ambientOcclusionIntensity);
            textureDisplayer.setPosition(TextureDisplayer::RIGHT, TextureDisplayer::BOTTOM);
            textureDisplayer.initialize(sceneWidth, sceneHeight, camera->getNearPlane(), camera->getFarPlane());
            textureDisplayer.display();
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
    void Renderer3d::deferredGeometryRendering(float dt) {
        ScopeProfiler profiler("3d", "defGeoRender");

        glClear((unsigned int)GL_DEPTH_BUFFER_BIT | (unsigned int)GL_COLOR_BUFFER_BIT);

        skyManager->display(camera->getViewMatrix(), camera->getPosition());

        updateModelsInFrustum();
        modelDisplayer->setModels(modelsInFrustum);

        modelDisplayer->display(camera->getViewMatrix());

        terrainManager->display(camera, dt);

        waterManager->display(camera, fogManager, dt);

        geometryManager->display(camera->getViewMatrix());

        if (isAmbientOcclusionActivated) {
            ambientOcclusionManager->updateAOTexture(camera);
        }

        displayGeometryDetails();
    }

    void Renderer3d::displayGeometryDetails() {
        if (DEBUG_DISPLAY_MODELS_OCTREE) {
            OctreeDisplayer::drawOctree(modelOctreeManager, camera->getProjectionMatrix(), camera->getViewMatrix());
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
            const Light *firstLight = lightManager->getVisibleLights()[0]; //choose light
            for (const auto &frustum : shadowManager->getSplitFrustums()) {
                shadowManager->drawLightSceneBox(frustum, firstLight, camera->getViewMatrix());
            }
        }
    }

    /**
     * Second pass of deferred shading algorithm.
     * Compute lighting in pixel shader and render the scene to screen.
     */
    void Renderer3d::lightingPassRendering() {
        ScopeProfiler profiler("3d", "lightPassRender");

        ShaderManager::instance()->bind(deferredShadingShader);
        unsigned int nextTextureUnit = 0;

        glActiveTexture(GL_TEXTURE0 + nextTextureUnit++);
        glBindTexture(GL_TEXTURE_2D, textureIDs[TEX_DEPTH]);

        glActiveTexture(GL_TEXTURE0 + nextTextureUnit++);
        glBindTexture(GL_TEXTURE_2D, textureIDs[TEX_DIFFUSE]);

        glActiveTexture(GL_TEXTURE0 + nextTextureUnit++);
        glBindTexture(GL_TEXTURE_2D, textureIDs[TEX_NORMAL_AND_AMBIENT]);

        glUniformMatrix4fv(mInverseViewProjectionLoc, 1, GL_FALSE, (const float*) (camera->getProjectionMatrix() * camera->getViewMatrix()).inverse());
        glUniform3fv(viewPositionLoc, 1, (const float *)camera->getPosition());

        lightManager->loadLights();

        if (isShadowActivated) {
            shadowManager->loadShadowMaps(nextTextureUnit);
            nextTextureUnit += shadowManager->getNumberShadowMaps();
        }

        if (isAmbientOcclusionActivated) {
            ambientOcclusionManager->loadAOTexture(nextTextureUnit);
            //nextTextureUnit++;
        }

        lightingPassDisplayer->display();
    }

    void Renderer3d::postUpdateScene() {
        ScopeProfiler profiler("3d", "postUpdateScene");

        modelOctreeManager->postRefreshOctreeables();

        lightManager->postUpdateLights();
    }

}
