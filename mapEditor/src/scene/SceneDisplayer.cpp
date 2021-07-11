#include <stdexcept>
#include <QMessageBox>
#include <unistd.h>

#include <scene/SceneDisplayer.h>
#include <scene/SceneWindowController.h>

namespace urchin {

    SceneDisplayer::SceneDisplayer(const std::unique_ptr<SceneWindowController>& windowController,
                                   SceneController* sceneController,
                                   const std::unique_ptr<MouseController>& mouseController,
                                   const std::unique_ptr<StatusBarController>& statusBarController) :
        isInitialized(false),
        windowController(windowController),
        sceneController(sceneController),
        mouseController(mouseController),
        statusBarController(statusBarController),
        sceneManager(std::unique_ptr<SceneManager>(nullptr)),
        camera(nullptr),
        bodyShapeDisplayer(std::unique_ptr<BodyShapeDisplayer>(nullptr)),
        objectMoveController(std::unique_ptr<ObjectMoveController>(nullptr)),
        lightScopeDisplayer(std::unique_ptr<LightScopeDisplayer>(nullptr)),
        soundTriggerDisplayer(std::unique_ptr<SoundTriggerDisplayer>(nullptr)),
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
        mapHandler.reset(nullptr);
        soundManager.reset(nullptr);
        aiManager.reset(nullptr);
        physicsWorld.reset(nullptr);

        bodyShapeDisplayer.reset(nullptr);
        objectMoveController.reset(nullptr);
        lightScopeDisplayer.reset(nullptr);
        soundTriggerDisplayer.reset(nullptr);
        delete camera;
        sceneManager.reset(nullptr);

        SingletonManager::destroyAllSingletons();
    }

    void SceneDisplayer::loadMap(const std::string& mapEditorPath, const std::string& mapFilename, const std::string& relativeWorkingDirectory) {
        try {
            initializeEngineResources(mapEditorPath);
            std::string mapResourcesDirectory = FileUtil::simplifyDirectoryPath(FileUtil::getDirectory(mapFilename) + relativeWorkingDirectory);
            FileSystem::instance()->setupResourcesDirectory(mapResourcesDirectory);

            initializeScene(mapFilename);

            mapHandler = std::make_unique<MapHandler>(sceneManager->getActiveRenderer3d(), physicsWorld.get(), soundManager.get(), aiManager.get());
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
            _exit(1);
        }
    }

    void SceneDisplayer::loadEmptyScene(const std::string& mapEditorPath) {
        initializeEngineResources(mapEditorPath);

        sceneManager = std::make_unique<SceneManager>(SceneWindowController::windowRequiredExtensions(), windowController->getSurfaceCreator(), windowController->getFramebufferSizeRetriever());
        sceneManager->newUIRenderer(true);
        sceneManager->getActiveUIRenderer()->addWidget(new StaticBitmap(nullptr, Position(0.0f, 0.0f, LengthType::PIXEL), Size(100.0f, 100.0f, LengthType::PERCENTAGE), "resources/emptyScene.tga"));

        isInitialized = true;
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
        sceneManager = std::make_unique<SceneManager>(SceneWindowController::windowRequiredExtensions(), windowController->getSurfaceCreator(), windowController->getFramebufferSizeRetriever());
        camera = new SceneFreeCamera(50.0f, 0.1f, 2000.0f, mouseController);
        camera->setSpeed(45.0f, 2.0f);
        camera->loadCameraState(mapFilename);
        sceneManager->newRenderer3d(true);
        sceneManager->getActiveRenderer3d()->setCamera(camera);
        sceneManager->getActiveRenderer3d()->getLightManager().setGlobalAmbientColor(Point4<float>(0.05f, 0.05f, 0.05f, 0.0f));

        bodyShapeDisplayer = std::make_unique<BodyShapeDisplayer>(*sceneManager);
        objectMoveController = std::make_unique<ObjectMoveController>(*sceneManager, sceneController, mouseController, statusBarController);
        lightScopeDisplayer = std::make_unique<LightScopeDisplayer>(*sceneManager);
        soundTriggerDisplayer = std::make_unique<SoundTriggerDisplayer>(*sceneManager);

        //physics
        physicsWorld = std::make_unique<PhysicsWorld>();
        AbstractBody::disableAllBodies(true);
        physicsWorld->setUp(1.0f / 50.0f);

        //AI
        aiManager = std::make_unique<AIManager>();
        aiManager->setUp(1.0f / 4.0f);
        navMeshDisplayer = std::make_unique<NavMeshDisplayer>(*aiManager, *sceneManager->getActiveRenderer3d());

        //sound
        soundManager = std::make_unique<SoundManager>();
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
        if (bodyShapeDisplayer && objectMoveController) {
            if (viewProperties[MODEL_PHYSICS]) {
                bodyShapeDisplayer->displayBodyShape();
            } else {
                bodyShapeDisplayer->clearDisplay();
            }

            objectMoveController->displayAxis();
        }
    }

    void SceneDisplayer::refreshLightScopeModel() {
        if (lightScopeDisplayer) {
            if (viewProperties[LIGHT_SCOPE] && highlightSceneLight && highlightSceneLight->getLight()) {
                lightScopeDisplayer->displayLightScopeFor(highlightSceneLight);
            } else {
                lightScopeDisplayer->displayLightScopeFor(nullptr);
            }
        }
    }

    void SceneDisplayer::refreshSoundTriggerModel() {
        if (soundTriggerDisplayer) {
            if (viewProperties[SOUND_TRIGGER] && highlightSceneSound && highlightSceneSound->getSoundTrigger()) {
                soundTriggerDisplayer->displaySoundTriggerFor(highlightSceneSound);
            } else {
                soundTriggerDisplayer->displaySoundTriggerFor(nullptr);
            }
        }
    }

    void SceneDisplayer::refreshNavMeshModel() {
        if (navMeshDisplayer) {
            if (viewProperties[NAV_MESH]) {
                navMeshDisplayer->display();
            } else {
                navMeshDisplayer->clearDisplay();
            }
        }
    }

    void SceneDisplayer::paint() {
        try {
            if (isInitialized) {
                if (mapHandler) {
                    mapHandler->refreshMap();
                }

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
            _exit(1);
        }
    }

    void SceneDisplayer::resize(unsigned int width, unsigned int height) {
        if (isInitialized) {
            sceneManager->onResize();
            if (objectMoveController) {
                objectMoveController->onResize(width, height);
            }
        }
    }

    SceneManager& SceneDisplayer::getSceneManager() const {
        assert(sceneManager);
        return *sceneManager;
    }

    SceneFreeCamera* SceneDisplayer::getCamera() const {
        return camera;
    }

    PhysicsWorld& SceneDisplayer::getPhysicsWorld() const {
        assert(physicsWorld);
        return *physicsWorld;
    }

    MapHandler& SceneDisplayer::getMapHandler() const {
        assert(mapHandler);
        return *mapHandler;
    }

    BodyShapeDisplayer* SceneDisplayer::getBodyShapeDisplayer() const {
        return bodyShapeDisplayer.get();
    }

    ObjectMoveController* SceneDisplayer::getObjectMoveController() const {
        return objectMoveController.get();
    }

}
