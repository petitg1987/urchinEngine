#include <stdexcept>

#include "SceneControllerWidget.h"

namespace urchin
{

	SceneControllerWidget::SceneControllerWidget(QWidget *parent) :
			QTabWidget(parent),
			sceneController(nullptr)
	{
		tabObjects = new ObjectControllerWidget();
		addTab(tabObjects, "Objects");

		tabLights = new LightControllerWidget();
		addTab(tabLights, "Lights");

		tabTerrains = new TerrainControllerWidget();
		addTab(tabTerrains, "Terrains");

		tabSounds = new SoundControllerWidget();
		addTab(tabSounds, "Sounds");

		tabAI = new AIControllerWidget();
		addTab(tabAI, "AI");

		connect(this, SIGNAL(currentChanged(int)), this, SLOT(tabSelected()));
		setCurrentIndex(0);
		setEnabled(false);
	}

	SceneControllerWidget::~SceneControllerWidget()
	{
		delete sceneController;
	}

	ObjectControllerWidget *SceneControllerWidget::getObjectControllerWidget() const
	{
		return tabObjects;
	}

	LightControllerWidget *SceneControllerWidget::getLightControllerWidget() const
	{
		return tabLights;
	}

	TerrainControllerWidget *SceneControllerWidget::getTerrainControllerWidget() const
	{
		return tabTerrains;
	}

	SoundControllerWidget *SceneControllerWidget::getSoundControllerWidget() const
	{
		return tabSounds;
	}

	AIControllerWidget *SceneControllerWidget::getAIControllerWidget() const
	{
		return tabAI;
	}

	bool SceneControllerWidget::isModified() const
	{
		return hasMapOpen() && sceneController->isModified();
	}

	bool SceneControllerWidget::hasMapOpen() const
	{
		return sceneController!=nullptr;
	}

	void SceneControllerWidget::newMap(MapHandler *mapHandler, const std::string &relativeWorkingDirectory)
	{
		closeMap();

		sceneController = new SceneController(mapHandler);
		sceneController->setRelativeWorkingDirectory(relativeWorkingDirectory);

		setEnabled(true);
		tabObjects->load(sceneController->getObjectController());
		tabLights->load(sceneController->getLightController());
		tabTerrains->load(sceneController->getTerrainController());
		tabSounds->load(sceneController->getSoundController());
		tabAI->load(sceneController->getAIController());
	}

	void SceneControllerWidget::openMap(MapHandler *mapHandler)
	{
		closeMap();

		sceneController = new SceneController(mapHandler);

		setEnabled(true);
		tabObjects->load(sceneController->getObjectController());
		tabLights->load(sceneController->getLightController());
		tabTerrains->load(sceneController->getTerrainController());
		tabSounds->load(sceneController->getSoundController());
		tabAI->load(sceneController->getAIController());
	}

	void SceneControllerWidget::saveMap(const std::string &mapFilename)
	{
		if(sceneController!=nullptr)
		{
			sceneController->saveMapOnFile(mapFilename);
		}
	}

	void SceneControllerWidget::closeMap()
	{
		tabObjects->unload();
		tabLights->unload();
		tabTerrains->unload();
		tabSounds->unload();
		tabAI->unload();
		setEnabled(false);

		delete sceneController;
		sceneController=nullptr;
	}

	SceneControllerWidget::TabName SceneControllerWidget::getTabSelected() const
	{
		int tabIndex = this->currentIndex();
		if(tabIndex==0)
		{
			return TabName::OBJECTS;
		}else if(tabIndex==1)
		{
			return TabName::LIGHTS;
		}else if(tabIndex==2)
		{
			return TabName ::TERRAINS;
		}else if(tabIndex==3)
		{
			return TabName::SOUNDS;
		}else if(tabIndex==4)
		{
			return TabName::AI;
		}

		throw std::runtime_error("Unknown selected tab index: " + std::to_string(tabIndex));
	}

	void SceneControllerWidget::tabSelected()
	{
		notifyObservers(this, NotificationType::TAB_SELECTED);
	}
}
