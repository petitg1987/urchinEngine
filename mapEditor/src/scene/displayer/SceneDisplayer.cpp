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
		soundManager(nullptr),
		aiManager(nullptr),
		camera(nullptr),
		mapHandler(nullptr),
		bodyShapeDisplayer(nullptr),
		lightScopeDisplayer(nullptr),
		soundTriggerDisplayer(nullptr),
		navMeshDisplayer(nullptr),
		highlightSceneObject(nullptr),
		highlightSceneLight(nullptr),
		highlightSceneSound(nullptr)
	{

	}

	SceneDisplayer::~SceneDisplayer()
	{
		delete mapHandler;

		delete camera;
		delete bodyShapeDisplayer;
		delete lightScopeDisplayer;
		delete soundTriggerDisplayer;
		delete navMeshDisplayer;
		delete sceneManager;
		delete soundManager;
		delete aiManager;

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

			initializeScene();

			mapHandler = new MapHandler(sceneManager->getActiveRenderer3d(), nullptr, soundManager, aiManager);
			std::string relativeMapFilename = FileHandler::getRelativePath(mapResourcesDirectory, mapFilename);
			mapHandler->loadMapFromFile(relativeMapFilename);
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

			initializeScene();
			mapHandler = new MapHandler(sceneManager->getActiveRenderer3d(), nullptr, soundManager, aiManager);

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
		std::string mapEditorResourcesDirectory = FileHandler::getDirectoryFrom(mapEditorPath);

		ConfigService::instance()->loadProperties("resources/engine.properties", mapEditorResourcesDirectory);
		ShaderManager::instance()->replaceShadersParentDirectoryBy(mapEditorResourcesDirectory);
	}

	void SceneDisplayer::initializeScene()
	{
		#ifdef _DEBUG
			if(isInitialized)
			{
				throw std::runtime_error("Scene displayer is already initialized.");
			}
		#endif

		sceneManager = new SceneManager();
		soundManager = new SoundManager();
		aiManager = new AIManager();
		aiManager->start(1.0/4.0, true);

		sceneManager->newRenderer3d(true);

		bodyShapeDisplayer = new BodyShapeDisplayer(sceneManager);
		lightScopeDisplayer = new LightScopeDisplayer(sceneManager);
		soundTriggerDisplayer = new SoundTriggerDisplayer(sceneManager);
		navMeshDisplayer = new NavMeshDisplayer(sceneManager);

		//3d scene configuration
		camera = new SceneFreeCamera(50.0f, 0.1f, 250.0f, parentWidget);
		camera->setDistance(0.0);
		camera->moveTo(Point3<float>(0.0, 0.0, 10.0));
		sceneManager->getActiveRenderer3d()->setCamera(camera);
		sceneManager->getActiveRenderer3d()->getLightManager()->setGlobalAmbientColor(Point4<float>(0.05, 0.05, 0.05, 0.0));
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

	void SceneDisplayer::refreshRigidBodyShapeModel()
	{
		if(viewProperties[MODEL_PHYSICS] && highlightSceneObject!=nullptr && highlightSceneObject->getRigidBody()!=nullptr)
		{
			bodyShapeDisplayer->displayBodyShapeFor(highlightSceneObject);
		}else
		{
			bodyShapeDisplayer->displayBodyShapeFor(nullptr);
		}
	}

	void SceneDisplayer::refreshLightScopeModel()
	{
		if(viewProperties[LIGHT_SCOPE] && highlightSceneLight!=nullptr && highlightSceneLight->getLight()!=nullptr)
		{
			lightScopeDisplayer->displayLightScopeFor(highlightSceneLight);
		}else
		{
			lightScopeDisplayer->displayLightScopeFor(nullptr);
		}
	}

	void SceneDisplayer::refreshSoundTriggerModel()
	{
		if(viewProperties[SOUND_TRIGGER] && highlightSceneSound!=nullptr && highlightSceneSound->getSoundTrigger()!=nullptr)
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
			navMeshDisplayer->displayNavMesh(aiManager->getNavMeshGenerator()->retrieveLastGeneratedNavMesh());
		}else
		{
			navMeshDisplayer->displayNavMesh(NavMesh());
		}
	}

	void SceneDisplayer::paint()
	{
		try
		{
			if(isInitialized)
			{
				mapHandler->refreshMap();

				refreshRigidBodyShapeModel();
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

	MapHandler *SceneDisplayer::getMapHandler() const
	{
		return mapHandler;
	}

}
