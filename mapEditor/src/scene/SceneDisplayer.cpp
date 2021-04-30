#include <stdexcept>
#include <QMessageBox>
#include <utility>

#include "SceneDisplayer.h"

namespace urchin {

    SceneDisplayer::SceneDisplayer(SceneController* sceneController, const MouseController& mouseController, StatusBarController statusBarController) :
        isInitialized(false),
        sceneController(sceneController),
        mouseController(mouseController),
        statusBarController(std::move(statusBarController)),
        sceneManager(nullptr),
        camera(nullptr),
        bodyShapeDisplayer(nullptr),
        objectMoveController(nullptr),
        lightScopeDisplayer(nullptr),
        soundTriggerDisplayer(nullptr),
        physicsWorld(nullptr),
        aiManager(nullptr),
        navMeshDisplayer(nullptr),
        soundManager(nullptr),
        mapHandler(nullptr),
        viewProperties(),
        highlightSceneObject(nullptr),
        highlightSceneLight(nullptr),
        highlightSceneSound(nullptr) {

    }

    SceneDisplayer::~SceneDisplayer() {
        delete mapHandler;

        delete soundManager;

        delete aiManager;

        delete physicsWorld;

        delete bodyShapeDisplayer;
        delete objectMoveController;
        delete lightScopeDisplayer;
        delete soundTriggerDisplayer;
        delete navMeshDisplayer;
        delete camera;
        delete sceneManager;

        SingletonManager::destroyAllSingletons();
    }

    void SceneDisplayer::loadMap(const std::string& mapEditorPath, const std::string& mapFilename, const std::string& relativeWorkingDirectory) {
        try {
            initializeEngineResources(mapEditorPath);
            std::string mapResourcesDirectory = FileUtil::simplifyDirectoryPath(FileUtil::getDirectory(mapFilename) + relativeWorkingDirectory);
            FileSystem::instance()->setupResourcesDirectory(mapResourcesDirectory);

            initializeScene(mapFilename);

            mapHandler = new MapHandler(sceneManager->getActiveRenderer3d(), physicsWorld, soundManager, aiManager);
            mapHandler->setRelativeWorkingDirectory(relativeWorkingDirectory);
            std::string relativeMapFilename = FileUtil::getRelativePath(mapResourcesDirectory, mapFilename);
            std::ifstream streamMapFile(FileSystem::instance()->getResourcesDirectory() + relativeMapFilename);
            if (streamMapFile) { //existing map
                NullLoadCallback nullLoadCallback;
                mapHandler->loadMapFromFile(relativeMapFilename, nullLoadCallback);
            }
            mapHandler->unpause();

            isInitialized = true;
        } catch (std::exception& e) {
            Logger::instance()->logError("Error occurred during map load: " + std::string(e.what()));
            QMessageBox::critical(nullptr, "Error", "Unexpected error occurred. Check log file for more details.");
            this->~SceneDisplayer();
            exit(1);
        }
    }

    void SceneDisplayer::initializeEngineResources(const std::string& mapEditorPath) {
        std::string mapEditorResourcesDirectory = FileUtil::getDirectory(mapEditorPath) + "resources/";

        ConfigService::instance()->loadProperties("engine.properties", mapEditorResourcesDirectory);
        ShaderConfig::instance()->replaceShadersParentDirectoryBy(mapEditorResourcesDirectory);
    }

    void SceneDisplayer::initializeScene(const std::string& mapFilename) {
        if (isInitialized) {
            throw std::runtime_error("Scene displayer is already initialized.");
        }

        //3d
        std::vector<const char*> windowRequiredExtensions;
        std::unique_ptr<SurfaceCreator> surfaceCreator;
        std::unique_ptr<FramebufferSizeRetriever> framebufferSizeRetriever;
        sceneManager = new SceneManager(windowRequiredExtensions, surfaceCreator, std::move(framebufferSizeRetriever)); //TODO review args
        camera = new SceneFreeCamera(50.0f, 0.1f, 2000.0f, mouseController);
        camera->setSpeed(45.0f, 2.0f);
        camera->loadCameraState(mapFilename);
        sceneManager->newRenderer3d(true);
        sceneManager->getActiveRenderer3d()->setCamera(camera);
        sceneManager->getActiveRenderer3d()->getLightManager()->setGlobalAmbientColor(Point4<float>(0.05f, 0.05f, 0.05f, 0.0f));

        bodyShapeDisplayer = new BodyShapeDisplayer(sceneManager);
        objectMoveController = new ObjectMoveController(sceneManager, sceneController, mouseController, statusBarController);
        lightScopeDisplayer = new LightScopeDisplayer(sceneManager);
        soundTriggerDisplayer = new SoundTriggerDisplayer(sceneManager);

        //physics
        physicsWorld = new PhysicsWorld();
        AbstractBody::disableAllBodies(true);
        physicsWorld->setUp(1.0f / 50.0f);

        //AI
        aiManager = new AIManager();
        aiManager->setUp(1.0f / 4.0f);
        navMeshDisplayer = new NavMeshDisplayer(aiManager, sceneManager->getActiveRenderer3d());

        //sound
        soundManager = new SoundManager();
    }

    void SceneDisplayer::setViewProperties(SceneDisplayer::ViewProperties viewProperty, bool value) {
        viewProperties[viewProperty] = value;
    }

    void SceneDisplayer::setHighlightSceneObject(const SceneObject* highlightSceneObject) {
        if (this->highlightSceneObject != highlightSceneObject) {
            this->highlightSceneObject = highlightSceneObject;

            bodyShapeDisplayer->setSelectedSceneObject(highlightSceneObject);
            objectMoveController->setSelectedSceneObject(highlightSceneObject);
        }
    }

    void SceneDisplayer::setHighlightSceneLight(const SceneLight* highlightSceneLight) {
        this->highlightSceneLight = highlightSceneLight;
    }

    void SceneDisplayer::setHighlightSceneSound(const SceneSound* highlightSceneSound) {
        this->highlightSceneSound = highlightSceneSound;
    }

    void SceneDisplayer::refreshObjectsModel() {
        if (viewProperties[MODEL_PHYSICS]) {
            bodyShapeDisplayer->displayBodyShape();
        } else {
            bodyShapeDisplayer->clearDisplay();
        }

        objectMoveController->displayAxis();
    }

    void SceneDisplayer::refreshLightScopeModel() {
        if (viewProperties[LIGHT_SCOPE] && highlightSceneLight && highlightSceneLight->getLight()) {
            lightScopeDisplayer->displayLightScopeFor(highlightSceneLight);
        } else {
            lightScopeDisplayer->displayLightScopeFor(nullptr);
        }
    }

    void SceneDisplayer::refreshSoundTriggerModel() {
        if (viewProperties[SOUND_TRIGGER] && highlightSceneSound && highlightSceneSound->getSoundTrigger()) {
            soundTriggerDisplayer->displaySoundTriggerFor(highlightSceneSound);
        } else {
            soundTriggerDisplayer->displaySoundTriggerFor(nullptr);
        }
    }

    void SceneDisplayer::refreshNavMeshModel() {
        if (viewProperties[NAV_MESH]) {
            navMeshDisplayer->display();
        } else {
            navMeshDisplayer->clearDisplay();
        }
    }

    void SceneDisplayer::paint() {
        try {
            if (isInitialized) {
                mapHandler->refreshMap();

                refreshObjectsModel();
                refreshLightScopeModel();
                refreshSoundTriggerModel();
                refreshNavMeshModel();

                sceneManager->display();
            }
        } catch (std::exception& e) {
            Logger::instance()->logError("Error occurred during paint: " + std::string(e.what()));
            QMessageBox::critical(nullptr, "Error", "Unexpected error occurred. Check log file for more details.");
            this->~SceneDisplayer();
            exit(1);
        }
    }

    void SceneDisplayer::resize(unsigned int width, unsigned int height) {
        if (isInitialized) {
            sceneManager->onResize(width, height);
            objectMoveController->onResize(width, height);
        }
    }

    SceneManager* SceneDisplayer::getSceneManager() const {
        return sceneManager;
    }

    SceneFreeCamera* SceneDisplayer::getCamera() const {
        return camera;
    }

    PhysicsWorld* SceneDisplayer::getPhysicsWorld() const {
        return physicsWorld;
    }

    MapHandler* SceneDisplayer::getMapHandler() const {
        return mapHandler;
    }

    BodyShapeDisplayer* SceneDisplayer::getBodyShapeDisplayer() const {
        return bodyShapeDisplayer;
    }

    ObjectMoveController* SceneDisplayer::getObjectMoveController() const {
        return objectMoveController;
    }

}
