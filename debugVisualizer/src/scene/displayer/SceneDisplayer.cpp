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

	void SceneDisplayer::initializeForOpen(const std::string &debugVisualizerPath, std::shared_ptr<GeometryEntityHandler> geometryEntityHandler)
	{
		try
		{
			this->geometryEntityHandler = geometryEntityHandler;

			initializeEngineResources(debugVisualizerPath);
			initializeScene();

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
		std::string debugVisualizerResourcesDirectory = FileHandler::getDirectoryFrom(mapEditorPath);

		FileSystem::instance()->setupResourcesDirectory(debugVisualizerResourcesDirectory + "resources/");
		ConfigService::instance()->loadProperties("engine.properties");
		ShaderManager::instance()->replaceShadersParentDirectoryBy(debugVisualizerResourcesDirectory);
	}

	void SceneDisplayer::initializeScene()
	{
		sceneManager = new SceneManager();
		sceneManager->newRenderer3d(true);

		//3d scene configuration
		camera = new SceneFreeCamera(50.0f, 0.1f, 500.0f, parentWidget);
		camera->setDistance(0.0);
		camera->moveTo(Point3<float>(0.0, 0.0, 10.0));
		sceneManager->getActiveRenderer3d()->setCamera(camera);
		sceneManager->getActiveRenderer3d()->getLightManager()->setGlobalAmbientColor(Point4<float>(0.05, 0.05, 0.05, 0.0));
		sceneManager->getActiveRenderer3d()->createSkybox({"skybox/NoTex.tga", "skybox/NoTex.tga", "skybox/NoTex.tga", "skybox/NoTex.tga", "skybox/NoTex.tga", "skybox/NoTex.tga"});
		sceneManager->getActiveRenderer3d()->getSkybox()->setOffsetY(-1.0f);
	}

	void SceneDisplayer::paint()
	{
		try
		{
			if(isInitialized)
			{
				sceneManager->getActiveRenderer3d()->removeAllGeometries();

				std::list<GeometryEntity *> geometryEntities = geometryEntityHandler->getGeometryEntities();
				for(std::list<GeometryEntity *>::const_iterator it = geometryEntities.begin(); it!=geometryEntities.end(); ++it)
				{
					sceneManager->getActiveRenderer3d()->addGeometry(((*it)->getGeometryModel()).get());
				}

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
