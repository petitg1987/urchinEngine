#include <stdexcept>

#include "SceneControllerWidget.h"

namespace urchin
{

	SceneControllerWidget::SceneControllerWidget(QWidget *parent) :
			QTabWidget(parent),
			sceneController(nullptr)
	{
		tabObjects = new ObjectControllerWidget();
		addTab(tabObjects, "Object");

		tabLights = new LightControllerWidget();
		addTab(tabLights, "Light");

		tabTerrains = new TerrainControllerWidget();
		addTab(tabTerrains, "Terrain");

		tabWaters = new WaterControllerWidget();
		addTab(tabWaters, "Water");

		tabSky = new SkyControllerWidget();
        addTab(tabSky, "Sky");

		tabSounds = new SoundControllerWidget();
		addTab(tabSounds, "Sound");

		tabAI = new AIControllerWidget();
		addTab(tabAI, "AI");

		connect(this, SIGNAL(currentChanged(int)), this, SLOT(tabSelected()));
		setCurrentIndex(0);
		setEnabled(false);
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

	WaterControllerWidget *SceneControllerWidget::getWaterControllerWidget() const
	{
		return tabWaters;
	}

    SkyControllerWidget *SceneControllerWidget::getSkyControllerWidget() const
    {
        return tabSky;
    }

	SoundControllerWidget *SceneControllerWidget::getSoundControllerWidget() const
	{
		return tabSounds;
	}

	AIControllerWidget *SceneControllerWidget::getAIControllerWidget() const
	{
		return tabAI;
	}

    SceneController *SceneControllerWidget::newMap(SceneController *sceneController)
	{
		closeMap();
		this->sceneController = sceneController;

		setEnabled(true);
		tabObjects->load(sceneController->getObjectController());
		tabLights->load(sceneController->getLightController());
		tabTerrains->load(sceneController->getTerrainController());
		tabWaters->load(sceneController->getWaterController());
		tabSky->load(sceneController->getSkyController());
		tabSounds->load(sceneController->getSoundController());
		tabAI->load(sceneController->getAIController());

		return sceneController;
	}

    SceneController *SceneControllerWidget::openMap(SceneController *sceneController)
	{
		closeMap();
        this->sceneController = sceneController;

		setEnabled(true);
		tabObjects->load(sceneController->getObjectController());
		tabLights->load(sceneController->getLightController());
		tabTerrains->load(sceneController->getTerrainController());
		tabWaters->load(sceneController->getWaterController());
        tabSky->load(sceneController->getSkyController());
		tabSounds->load(sceneController->getSoundController());
		tabAI->load(sceneController->getAIController());

		return sceneController;
	}

	void SceneControllerWidget::closeMap()
	{
		tabObjects->unload();
		tabLights->unload();
		tabTerrains->unload();
		tabWaters->unload();
		tabSky->unload();
		tabSounds->unload();
		tabAI->unload();
		setEnabled(false);

		sceneController = nullptr;
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
			return TabName::TERRAINS;
		}else if(tabIndex==3)
        {
            return TabName::WATERS;
        }else if(tabIndex==4)
        {
            return TabName::SKY;
		}else if(tabIndex==5)
		{
			return TabName::SOUNDS;
		}else if(tabIndex==6)
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
