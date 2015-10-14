#include <memory>
#include <QMessageBox>
#include <QWidget>

#include "SceneDisplayer.h"

namespace urchin
{

	SceneDisplayer::SceneDisplayer(QWidget *parentWidget) :
		  parentWidget(parentWidget),
		  sceneManager(nullptr),
		  camera(nullptr),
		  mapHandler(nullptr),
		  bodyShapeDisplayer(nullptr),
		  lightScopeDisplayer(nullptr),
		  soundTriggerDisplayer(nullptr),
		  highlightSceneObject(nullptr),
		  highlightSceneLight(nullptr),
		  highlightSceneSound(nullptr)
	{

	}

	SceneDisplayer::~SceneDisplayer()
	{
		delete camera;
		delete bodyShapeDisplayer;
		delete lightScopeDisplayer;
		delete soundTriggerDisplayer;
		delete sceneManager;

		SingletonManager::destroyAllSingletons();
	}

	void SceneDisplayer::initializeFromExistingMap(const std::string &mapEditorPath, const std::string &mapFilename)
	{
		try
		{
			initializeEngineResources(mapEditorPath);
			std::string relativeMapWorkingDir = MapHandler::getRelativeWorkingDirectory(mapFilename);
			std::string mapWorkingDirectory = FileHandler::simplifyDirectoryPath(FileHandler::getDirectoryFrom(mapFilename) + relativeMapWorkingDir);
			FileSystem::instance()->setupWorkingDirectory(mapWorkingDirectory);

			initializeScene();
			std::string relativeMapFilename = FileHandler::getRelativePath(mapWorkingDirectory, mapFilename);
			mapHandler = new MapHandler(sceneManager, nullptr, nullptr, relativeMapFilename);
		}catch(std::exception &e)
		{
			QMessageBox::critical(nullptr, "Error", e.what());
			this->~SceneDisplayer();
			exit(1);
		}
	}

	void SceneDisplayer::initializeFromNewMap(const std::string &mapEditorPath, const std::string &mapFilename, const std::string &relativeMapWorkingDir)
	{
		try
		{
			initializeEngineResources(mapEditorPath);
			std::string mapWorkingDirectory = FileHandler::simplifyDirectoryPath(FileHandler::getDirectoryFrom(mapFilename) + relativeMapWorkingDir);
			FileSystem::instance()->setupWorkingDirectory(mapWorkingDirectory);

			initializeScene();
			mapHandler = new MapHandler(sceneManager, nullptr, nullptr);
		}catch(std::exception &e)
		{
			QMessageBox::critical(nullptr, "Error", e.what());
			this->~SceneDisplayer();
			exit(1);
		}
	}

	void SceneDisplayer::initializeEngineResources(const std::string &mapEditorPath)
	{
		std::string mapEditorWorkingDirectory = FileHandler::getDirectoryFrom(mapEditorPath);

		ConfigService::instance()->loadProperties("resources/engine.properties", mapEditorWorkingDirectory);
		ShaderManager::instance()->replaceShadersWorkingDirectoryBy(mapEditorWorkingDirectory);
	}

	void SceneDisplayer::initializeScene()
	{
		sceneManager = new SceneManager();
		sceneManager->initialize();

		bodyShapeDisplayer = new BodyShapeDisplayer(sceneManager);
		lightScopeDisplayer = new LightScopeDisplayer(sceneManager);
		soundTriggerDisplayer = new SoundTriggerDisplayer(sceneManager);

		//3d scene configuration
		camera = new SceneFreeCamera(50.0f, 0.1f, 250.0f, parentWidget);
		camera->setDistance(0.0);
		camera->moveTo(Point3<float>(0.0, 0.0, 10.0));
		sceneManager->get3dRenderer()->setCamera(camera);
		sceneManager->get3dRenderer()->getLightManager()->setGlobalAmbientColor(Point4<float>(0.6, 0.6, 0.3, 0.0));
	}

	void SceneDisplayer::setViewProperties(SceneDisplayer::ViewProperties viewProperty, bool value)
	{
		viewProperties[viewProperty] = value;
	}

	void SceneDisplayer::setHighlightSceneObject(const SceneObject *highlightSceneObject)
	{
		this->highlightSceneObject = highlightSceneObject;
	}

	void SceneDisplayer::setHighlightCompoundShapeComponent(const LocalizedCollisionShape *selectedCompoundShapeComponent)
	{
		bodyShapeDisplayer->setSelectedCompoundShapeComponent(selectedCompoundShapeComponent);
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

	void SceneDisplayer::paint()
	{
		try
		{
			refreshRigidBodyShapeModel();
			refreshLightScopeModel();
			refreshSoundTriggerModel();

			sceneManager->display();
		}catch(std::exception &e)
		{
			QMessageBox::critical(nullptr, "Error", e.what());
			this->~SceneDisplayer();
			exit(1);
		}
	}

	void SceneDisplayer::resize(int width, int height)
	{
		if(sceneManager!=nullptr)
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
