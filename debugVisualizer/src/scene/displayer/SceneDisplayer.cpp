#include <memory>
#include <QMessageBox>
#include <QWidget>

#include "SceneDisplayer.h"

namespace urchin
{

	SceneDisplayer::SceneDisplayer(QWidget *parentWidget) :
		isInitialized(false),
		parentWidget(parentWidget),
		sceneManager(nullptr),
		camera(nullptr)
	{

	}

	SceneDisplayer::~SceneDisplayer()
	{
		delete camera;
		delete sceneManager;

		SingletonManager::destroyAllSingletons();
	}

	void SceneDisplayer::initializeFromDebugFile(const std::string &debugVisualizerPath, const std::string &debugFilename)
	{
		try
		{
			initializeEngineResources(debugVisualizerPath);
			std::string relativeMapResourcesDir = MapHandler::getRelativeWorkingDirectory(debugFilename);
			std::string mapResourcesDirectory = FileHandler::simplifyDirectoryPath(FileHandler::getDirectoryFrom(debugFilename) + relativeMapResourcesDir);
			FileSystem::instance()->setupResourcesDirectory(mapResourcesDirectory);

			initializeScene();
			std::string relativeMapFilename = FileHandler::getRelativePath(mapResourcesDirectory, debugFilename);

			//TODO read file & load in sceneManager->getActiveRenderer3d()

			isInitialized = true;
		}catch(std::exception &e)
		{
			QMessageBox::critical(nullptr, "Error", e.what());
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
		sceneManager = new SceneManager();
		sceneManager->newRenderer3d(true);

		//3d scene configuration
		camera = new SceneFreeCamera(50.0f, 0.1f, 250.0f, parentWidget);
		camera->setDistance(0.0);
		camera->moveTo(Point3<float>(0.0, 0.0, 10.0));
		sceneManager->getActiveRenderer3d()->setCamera(camera);
		sceneManager->getActiveRenderer3d()->getLightManager()->setGlobalAmbientColor(Point4<float>(0.05, 0.05, 0.05, 0.0));
	}

	void SceneDisplayer::paint()
	{
		try
		{
			if(isInitialized)
			{
				sceneManager->display();
			}
		}catch(std::exception &e)
		{
			QMessageBox::critical(nullptr, "Error", e.what());
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

}
