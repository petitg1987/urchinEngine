#include <stdexcept>
#include <QMessageBox>
#include <unistd.h>
#include <UrchinAggregation.h>

#include <scene/SceneDisplayer.h>
#include <scene/SceneWindowController.h>

namespace urchin {

    SceneDisplayer::SceneDisplayer(SceneWindowController& windowController, SceneController* sceneController, MouseController& mouseController,
                                   StatusBarController& statusBarController) :
            isInitialized(false),
            windowController(windowController),
            sceneController(sceneController),
            mouseController(mouseController),
            statusBarController(statusBarController),
            viewProperties(),
            highlightObjectMesh(nullptr),
            highlightObjectLight(nullptr),
            highlightObjectSound(nullptr) {

    }

    SceneDisplayer::~SceneDisplayer() {
        map.reset(nullptr);
        soundEnvironment.reset(nullptr);
        aiEnvironment.reset(nullptr);
        physicsWorld.reset(nullptr);

        bodyShapeDisplayer.reset(nullptr);
        objectMoveController.reset(nullptr);
        lightScopeDisplayer.reset(nullptr);
        soundScopeDisplayer.reset(nullptr);
        navMeshDisplayer.reset(nullptr);
        scene.reset(nullptr);
    }

    void SceneDisplayer::loadMap(const std::string& mapEditorPath, const std::string& mapFilename, const std::string& relativeWorkingDirectory) {
        try {
            initializeEngineResources(mapEditorPath);
            std::string mapResourcesDirectory = PathUtil::simplifyDirectoryPath(FileUtil::getDirectory(mapFilename) + relativeWorkingDirectory);
            FileSystem::instance().setupResourcesDirectory(mapResourcesDirectory);

            initializeScene(mapFilename);

            map = std::make_unique<Map>(scene->getActiveRenderer3d(), physicsWorld.get(), soundEnvironment.get(), aiEnvironment.get());
            if (FileUtil::isFileExist(mapFilename)) { //existing map
                LoadMapCallback nullLoadCallback;
                MapSaveService().loadMap(mapFilename, nullLoadCallback, *map);
            } else {
                map->setRelativeWorkingDirectory(relativeWorkingDirectory);
            }
            map->unpause();

            isInitialized = true;
        } catch (const std::exception& e) {
            Logger::instance().logError("Error occurred during map load: " + std::string(e.what()));
            QMessageBox::critical(nullptr, "Error", "Unexpected error occurred. Check log file for more details.");
            this->~SceneDisplayer();
            _exit(1);
        }
    }

    void SceneDisplayer::loadEmptyScene(const std::string& mapEditorPath) {
        initializeEngineResources(mapEditorPath);

        scene = std::make_unique<Scene>(SceneWindowController::windowRequiredExtensions(), windowController.getSurfaceCreator(), windowController.getFramebufferSizeRetriever(), true);
        scene->newUIRenderer(true);
        scene->getActiveUIRenderer()->addWidget(StaticBitmap::create(nullptr, Position(0.0f, 0.0f, PIXEL), Size(100.0f, 100.0f, SCREEN_PERCENT), "resources/emptyScene.png"));

        isInitialized = true;
    }

    void SceneDisplayer::initializeEngineResources(const std::string& mapEditorPath) {
        std::string mapEditorResourcesDirectory = FileUtil::getDirectory(mapEditorPath) + "resources/";

        ConfigService::instance().loadProperties("engine.properties", mapEditorResourcesDirectory);
        ShaderConfig::instance().replaceShadersParentDirectoryBy(mapEditorResourcesDirectory);
    }

    void SceneDisplayer::initializeScene(const std::string& mapFilename) {
        if (isInitialized) {
            throw std::runtime_error("Scene displayer is already initialized.");
        }

        //3d
        scene = std::make_unique<Scene>(SceneWindowController::windowRequiredExtensions(), windowController.getSurfaceCreator(), windowController.getFramebufferSizeRetriever(), true);
        camera = std::make_shared<SceneFreeCamera>(90.0f, 0.1f, 2000.0f, mouseController);
        camera->setSpeed(15.0f, 2.0f);
        camera->loadCameraState(mapFilename);
        scene->newRenderer3d(camera, VisualConfig(), true);
        scene->getActiveRenderer3d()->getLightManager().setGlobalAmbientColor(Point3(0.05f, 0.05f, 0.05f));

        bodyShapeDisplayer = std::make_unique<BodyShapeDisplayer>(*scene);
        objectMoveController = std::make_unique<ObjectMoveController>(*scene, *sceneController, mouseController, statusBarController);
        lightScopeDisplayer = std::make_unique<LightScopeDisplayer>(*scene);
        soundScopeDisplayer = std::make_unique<SoundScopeDisplayer>(*scene);

        //physics
        physicsWorld = std::make_unique<PhysicsWorld>();
        AbstractBody::disableAllBodies(true);
        physicsWorld->setUp(1.0f / 50.0f);

        //AI
        aiEnvironment = std::make_unique<AIEnvironment>();
        aiEnvironment->setUp(1.0f / 4.0f);
        navMeshDisplayer = std::make_unique<NavMeshDisplayer>(*aiEnvironment, *scene->getActiveRenderer3d());

        //sound
        soundEnvironment = std::make_unique<SoundEnvironment>();
    }

    void SceneDisplayer::setViewProperties(ViewProperties viewProperty, bool value) {
        viewProperties[viewProperty] = value;
    }

    void SceneDisplayer::setHighlightObjectMesh(const ObjectEntity* highlightObjectMesh) {
        if (this->highlightObjectMesh != highlightObjectMesh) {
            this->highlightObjectMesh = highlightObjectMesh;

            bodyShapeDisplayer->setSelectedObjectEntity(highlightObjectMesh);
            objectMoveController->setSelectedObjectEntity(highlightObjectMesh);
        }
    }

    void SceneDisplayer::setHighlightObjectLight(const ObjectEntity* highlightObjectLight) {
        this->highlightObjectLight = highlightObjectLight;
    }

    void SceneDisplayer::setHighlightObjectSound(const ObjectEntity* highlightObjectSound) {
        this->highlightObjectSound = highlightObjectSound;
    }

    void SceneDisplayer::refreshObjectsModel() const {
        if (bodyShapeDisplayer && objectMoveController) {
            if (viewProperties[OBJECT_SCOPE]) {
                bodyShapeDisplayer->displayBodyShape();
            } else {
                bodyShapeDisplayer->clearDisplay();
            }

            objectMoveController->displayAxis();
        }
    }

    void SceneDisplayer::refreshLightScopeModel() const {
        if (lightScopeDisplayer) {
            if (viewProperties[LIGHT_SCOPE] && highlightObjectLight && highlightObjectLight->getLight()) {
                lightScopeDisplayer->displayLightScope(highlightObjectLight);
            } else {
                lightScopeDisplayer->displayLightScope(nullptr);
            }
        }
    }

    void SceneDisplayer::refreshSoundTriggerModel() const {
        if (soundScopeDisplayer) {
            if (viewProperties[SOUND_SCOPE] && highlightObjectSound && highlightObjectSound->getSoundComponent()) {
                soundScopeDisplayer->displaySoundScope(highlightObjectSound);
            } else {
                soundScopeDisplayer->displaySoundScope(nullptr);
            }
        }
    }

    void SceneDisplayer::refreshNavMeshModel() const {
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
                if (map) {
                    map->refresh();
                }

                refreshObjectsModel();
                refreshLightScopeModel();
                refreshSoundTriggerModel();
                refreshNavMeshModel();

                scene->display();
            }
        } catch (const std::exception& e) {
            Logger::instance().logError("Error occurred during paint: " + std::string(e.what()));
            QMessageBox::critical(nullptr, "Error", "Unexpected error occurred. Check log file for more details.");
            this->~SceneDisplayer();
            _exit(1);
        }
    }

    void SceneDisplayer::resize(unsigned int width, unsigned int height) const {
        if (isInitialized) {
            scene->onResize();
            if (objectMoveController) {
                objectMoveController->onResize(width, height);
            }
        }
    }

    Scene& SceneDisplayer::getScene() const {
        assert(scene);
        return *scene;
    }

    SceneFreeCamera* SceneDisplayer::getCamera() const {
        return camera.get();
    }

    PhysicsWorld& SceneDisplayer::getPhysicsWorld() const {
        assert(physicsWorld);
        return *physicsWorld;
    }

    Map& SceneDisplayer::getMap() const {
        assert(map);
        return *map;
    }

    BodyShapeDisplayer* SceneDisplayer::getBodyShapeDisplayer() const {
        return bodyShapeDisplayer.get();
    }

    ObjectMoveController* SceneDisplayer::getObjectMoveController() const {
        return objectMoveController.get();
    }

}
