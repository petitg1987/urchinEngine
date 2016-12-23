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

		tabSounds = new SoundControllerWidget();
		addTab(tabSounds, "Sounds");

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

	SoundControllerWidget *SceneControllerWidget::getSoundControllerWidget() const
	{
		return tabSounds;
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
		tabSounds->load(sceneController->getSoundController());
	}

	void SceneControllerWidget::openMap(MapHandler *mapHandler)
	{
		closeMap();

		sceneController = new SceneController(mapHandler);

		setEnabled(true);
		tabObjects->load(sceneController->getObjectController());
		tabLights->load(sceneController->getLightController());
		tabSounds->load(sceneController->getSoundController());
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
		tabSounds->unload();
		setEnabled(false);

		delete sceneController;
		sceneController=nullptr;
	}

	int SceneControllerWidget::getTabSelected() const
	{
		return this->currentIndex();
	}

	void SceneControllerWidget::tabSelected()
	{
		notifyObservers(this, NotificationType::TAB_SELECTED);
	}
}
