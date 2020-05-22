#include <stdexcept>
#include <memory>
#include <QMessageBox>

#include "SceneDisplayer.h"

namespace urchin
{

	SceneDisplayer::SceneDisplayer(QWidget *parentWidget) :
		isInitialized(false),
		parentWidget(parentWidget),
		sceneManager(nullptr),
		physicsWorld(nullptr),
		soundManager(nullptr),
		aiManager(nullptr),
		camera(nullptr),
		mapHandler(nullptr),
		bodyShapeDisplayer(nullptr),
        objectMoveAxisDisplayer(nullptr),
		lightScopeDisplayer(nullptr),
		soundTriggerDisplayer(nullptr),
		navMeshDisplayer(nullptr),
        viewProperties(),
		highlightSceneObject(nullptr),
		highlightSceneLight(nullptr),
		highlightSceneSound(nullptr)
	{

	}

	SceneDisplayer::~SceneDisplayer()
	{
		delete mapHandler;

        delete soundManager;

        delete aiManager;

        delete physicsWorld;

        delete bodyShapeDisplayer;
        delete objectMoveAxisDisplayer;
        delete lightScopeDisplayer;
        delete soundTriggerDisplayer;
        delete navMeshDisplayer;
		delete camera;
		delete sceneManager;

		SingletonManager::destroyAllSingletons();
	}

	void SceneDisplayer::initializeFromExistingMap(const std::string &mapEditorPath, const std::string &mapFilename)
	{
		try
		{
			initializeEngineResources(mapEditorPath);
			std::string relativeMapResourcesDir = MapHandler::getRelativeWorkingDirectory(mapFilename);
			std::string mapResourcesDirectory = FileHandler::simplifyDirectoryPath(FileHandler::getDirectoryFrom(mapFilename) + relativeMapResourcesDir);
			FileSystem::instance()->setupResourcesDirectory(mapResourcesDirectory);

			initializeScene(mapFilename);

			mapHandler = new MapHandler(sceneManager->getActiveRenderer3d(), physicsWorld, soundManager, aiManager);
			std::string relativeMapFilename = FileHandler::getRelativePath(mapResourcesDirectory, mapFilename);
			NullLoadCallback nullLoadCallback;
			mapHandler->loadMapFromFile(relativeMapFilename, nullLoadCallback);
			physicsWorld->play();
			aiManager->play();

			isInitialized = true;
		}catch(std::exception &e)
		{
			Logger::logger().logError("Error occurred during map initialization: " + std::string(e.what()));
            QMessageBox::critical(nullptr, "Error", "Unexpected error occurred. Check log file for more details.");
			this->~SceneDisplayer();
			exit(1);
		}
	}

	void SceneDisplayer::initializeFromNewMap(const std::string &mapEditorPath, const std::string &mapFilename, const std::string &relativeMapResourcesDir)
	{
		try
		{
			initializeEngineResources(mapEditorPath);
			std::string mapResourcesDirectory = FileHandler::simplifyDirectoryPath(FileHandler::getDirectoryFrom(mapFilename) + relativeMapResourcesDir);
			FileSystem::instance()->setupResourcesDirectory(mapResourcesDirectory);

			initializeScene(mapFilename);
			mapHandler = new MapHandler(sceneManager->getActiveRenderer3d(), physicsWorld, soundManager, aiManager);

			isInitialized = true;
		}catch(std::exception &e)
		{
			Logger::logger().logError("Error occurred during map creation: " + std::string(e.what()));
			QMessageBox::critical(nullptr, "Error", "Unexpected error occurred. Check log file for more details.");
			this->~SceneDisplayer();
			exit(1);
		}
	}

	void SceneDisplayer::initializeEngineResources(const std::string &mapEditorPath)
	{
		std::string mapEditorResourcesDirectory = FileHandler::getDirectoryFrom(mapEditorPath) + "resources/";
		std::string mapEditorSaveDirectory = FileHandler::getDirectoryFrom(mapEditorPath) + "save/";

		ConfigService::instance()->loadProperties("engine.properties", mapEditorResourcesDirectory);
		ShaderManager::instance()->replaceShadersParentDirectoryBy(mapEditorResourcesDirectory);
		FileSystem::instance()->setupSaveDirectory(mapEditorSaveDirectory);
	}

	void SceneDisplayer::initializeScene(const std::string &mapFilename)
	{
		if(isInitialized)
		{
			throw std::runtime_error("Scene displayer is already initialized.");
		}

        //3d
        sceneManager = new SceneManager();
        camera = new SceneFreeCamera(50.0f, 0.1f, 2000.0f, parentWidget);
		camera->setSpeed(45.0f, 2.0f);
		camera->loadCameraState(mapFilename);
        sceneManager->newRenderer3d(true);
        sceneManager->getActiveRenderer3d()->setCamera(camera);
        sceneManager->getActiveRenderer3d()->getLightManager()->setGlobalAmbientColor(Point4<float>(0.05, 0.05, 0.05, 0.0));

        bodyShapeDisplayer = new BodyShapeDisplayer(sceneManager);
        objectMoveAxisDisplayer = new ObjectMoveAxisDisplayer(sceneManager);
        lightScopeDisplayer = new LightScopeDisplayer(sceneManager);
        soundTriggerDisplayer = new SoundTriggerDisplayer(sceneManager);

        //physics
		physicsWorld = new PhysicsWorld();
        AbstractWorkBody::disableAllBodies(true);
		physicsWorld->start(1.0/50.0, true);

        //AI
		aiManager = new AIManager();
		aiManager->start(1.0/4.0, true);
        navMeshDisplayer = new NavMeshDisplayer(aiManager, sceneManager->getActiveRenderer3d());

        //sound
        soundManager = new SoundManager();
	}

	void SceneDisplayer::setViewProperties(SceneDisplayer::ViewProperties viewProperty, bool value)
	{
		viewProperties[viewProperty] = value;
	}

	void SceneDisplayer::setHighlightSceneObject(const SceneObject *highlightSceneObject)
	{
		this->highlightSceneObject = highlightSceneObject;
	}

	void SceneDisplayer::setHighlightCompoundShapeComponent(std::shared_ptr<const LocalizedCollisionShape> selectedCompoundShapeComponent)
	{
		bodyShapeDisplayer->setSelectedCompoundShapeComponent(std::move(selectedCompoundShapeComponent));
	}

	void SceneDisplayer::setHighlightSceneLight(const SceneLight *highlightSceneLight)
	{
		this->highlightSceneLight = highlightSceneLight;
	}

	void SceneDisplayer::setHighlightSceneSound(const SceneSound *highlightSceneSound)
	{
		this->highlightSceneSound = highlightSceneSound;
	}

	void SceneDisplayer::refreshObjectsModel()
	{
        if(viewProperties[MODEL_PHYSICS] && highlightSceneObject && highlightSceneObject->getRigidBody())
        {
            bodyShapeDisplayer->displayBodyShapeFor(highlightSceneObject);
        }else
        {
            bodyShapeDisplayer->displayBodyShapeFor(nullptr);
        }

        objectMoveAxisDisplayer->displayAxisFor(highlightSceneObject);
	}

	void SceneDisplayer::refreshLightScopeModel()
	{
		if(viewProperties[LIGHT_SCOPE] && highlightSceneLight && highlightSceneLight->getLight())
		{
			lightScopeDisplayer->displayLightScopeFor(highlightSceneLight);
		}else
		{
			lightScopeDisplayer->displayLightScopeFor(nullptr);
		}
	}

	void SceneDisplayer::refreshSoundTriggerModel()
	{
		if(viewProperties[SOUND_TRIGGER] && highlightSceneSound && highlightSceneSound->getSoundTrigger())
		{
			soundTriggerDisplayer->displaySoundTriggerFor(highlightSceneSound);
		}else
		{
			soundTriggerDisplayer->displaySoundTriggerFor(nullptr);
		}
	}

	void SceneDisplayer::refreshNavMeshModel()
	{
		if(viewProperties[NAV_MESH])
		{
			navMeshDisplayer->display();
		}else
		{
			navMeshDisplayer->clearDisplay();
		}
	}

	void SceneDisplayer::paint()
	{
		try
		{
			if(isInitialized)
			{
				mapHandler->refreshMap();

                refreshObjectsModel();
				refreshLightScopeModel();
				refreshSoundTriggerModel();
				refreshNavMeshModel();

				sceneManager->display();
			}
		}catch(std::exception &e)
		{
			Logger::logger().logError("Error occurred during paint: " + std::string(e.what()));
			QMessageBox::critical(nullptr, "Error", "Unexpected error occurred. Check log file for more details.");
			this->~SceneDisplayer();
			exit(1);
		}
	}

	void SceneDisplayer::resize(unsigned int width, unsigned int height)
	{
		if(isInitialized)
		{
			sceneManager->onResize(width, height);
		}
	}

	SceneManager *SceneDisplayer::getSceneManager() const
	{
		return sceneManager;
	}

    SceneFreeCamera *SceneDisplayer::getCamera() const
    {
        return camera;
    }

    PhysicsWorld *SceneDisplayer::getPhysicsWorld() const
    {
        return physicsWorld;
    }

	MapHandler *SceneDisplayer::getMapHandler() const
	{
		return mapHandler;
	}

}
