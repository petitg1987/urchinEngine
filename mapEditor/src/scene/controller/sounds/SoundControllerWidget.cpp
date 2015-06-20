#include "SoundControllerWidget.h"
#include "support/GroupBoxStyleHelper.h"
#include "support/SpinBoxStyleHelper.h"
#include "scene/controller/sounds/dialog/NewSoundDialog.h"
#include "scene/controller/sounds/dialog/ChangeSoundTriggerDialog.h"
#include "scene/controller/sounds/dialog/ChangeSoundShapeDialog.h"
#include "scene/controller/sounds/soundshape/SoundShapeWidgetRetriever.h"

namespace urchin
{

	SoundControllerWidget::SoundControllerWidget() :
			soundController(nullptr),
			disableSoundEvent(false)
	{
		soundTableView = new SoundTableView(this);
		soundTableView->addObserver(this, SoundTableView::SELECTION_CHANGED);
		soundTableView->setGeometry(QRect(0, 0, 355, 220));

		addSoundButton = new QPushButton(this);
		addSoundButton->setText("New Sound");
		addSoundButton->setGeometry(QRect(0, 221, 85, 22));
		connect(addSoundButton, SIGNAL(clicked()), this, SLOT(showAddSoundDialog()));

		removeSoundButton = new QPushButton(this);
		removeSoundButton->setText("Remove Sound");
		removeSoundButton->setGeometry(QRect(86, 221, 85, 22));
		removeSoundButton->setEnabled(false);
		connect(removeSoundButton, SIGNAL(clicked()), this, SLOT(removeSelectedSound()));

		soundGroupBox = new QGroupBox(this);
		soundGroupBox->setTitle("Sound");
		soundGroupBox->setGeometry(QRect(0, 250, 355, 160));
		GroupBoxStyleHelper::applyNormalStyle(soundGroupBox);
		soundGroupBox->hide();

		setupSoundGeneralPropertiesBox(soundGroupBox);
		setupSoundSpecificPropertiesBox(soundGroupBox);

		soundTriggerGroupBox = new QGroupBox(this);
		soundTriggerGroupBox->setTitle("Sound Trigger");
		soundTriggerGroupBox->setGeometry(QRect(0, 415, 355, 360));
		GroupBoxStyleHelper::applyNormalStyle(soundTriggerGroupBox);
		soundTriggerGroupBox->hide();

		setupSoundBehaviorPropertiesBox(soundTriggerGroupBox);
		setupSoundTriggerSpecificPropertiesBox(soundTriggerGroupBox);
	}

	SoundControllerWidget::~SoundControllerWidget()
	{

	}

	void SoundControllerWidget::setupSoundGeneralPropertiesBox(QWidget *lightBox)
	{
		QGroupBox *generalPropertiesGroupBox = new QGroupBox(lightBox);
		generalPropertiesGroupBox->setTitle("General Properties");
		generalPropertiesGroupBox->setGeometry(QRect(5, 15, 345, 40));
		GroupBoxStyleHelper::applyNormalStyle(generalPropertiesGroupBox);

		QLabel *volumeLabel= new QLabel(generalPropertiesGroupBox);
		volumeLabel->setText("Volume:");
		volumeLabel->setGeometry(QRect(5, 15, 55, 22));

		volume = new QDoubleSpinBox(generalPropertiesGroupBox);
		volume->setGeometry(QRect(85, 15, 80, 22));
		SpinBoxStyleHelper::applyNormalStyle(volume);
		volume->setMinimum(0.0);
		connect(volume, SIGNAL(valueChanged(double)), this, SLOT(updateSoundGeneralProperties()));
	}

	void SoundControllerWidget::setupSoundSpecificPropertiesBox(QWidget *soundBox)
	{
		QGroupBox *specificPropertiesGroupBox = new QGroupBox(soundBox);
		specificPropertiesGroupBox->setTitle("Specific Properties");
		specificPropertiesGroupBox->setGeometry(QRect(5, 60, 345, 95));
		GroupBoxStyleHelper::applyNormalStyle(specificPropertiesGroupBox);

		QLabel *soundTypeLabel= new QLabel(specificPropertiesGroupBox);
		soundTypeLabel->setText("Sound Type:");
		soundTypeLabel->setGeometry(QRect(5, 15, 65, 22));

		soundType = new QLabel(specificPropertiesGroupBox);
		soundType->setGeometry(QRect(85, 15, 80, 22));

		setupAmbientSoundBox(specificPropertiesGroupBox);
		setupPointSoundBox(specificPropertiesGroupBox);
	}

	void SoundControllerWidget::setupAmbientSoundBox(QWidget *specificPropertiesGroupBox)
	{
		ambientSoundWidget = new QWidget(specificPropertiesGroupBox);
		ambientSoundWidget->setGeometry(QRect(5, 40, 345, 85));
		ambientSoundWidget->hide();
	}

	void SoundControllerWidget::setupPointSoundBox(QWidget *specificPropertiesGroupBox)
	{
		pointSoundWidget = new QWidget(specificPropertiesGroupBox);
		pointSoundWidget->setGeometry(QRect(5, 40, 345, 85));
		pointSoundWidget->hide();

		QLabel *positionLabel= new QLabel(pointSoundWidget);
		positionLabel->setText("Position:");
		positionLabel->setGeometry(QRect(0, 0, 55, 22));

		positionX = new QDoubleSpinBox(pointSoundWidget);
		positionX->setGeometry(QRect(85, 0, 80, 22));
		SpinBoxStyleHelper::applyNormalStyle(positionX);
		connect(positionX, SIGNAL(valueChanged(double)), this, SLOT(updateSoundSpecificProperties()));
		positionY = new QDoubleSpinBox(pointSoundWidget);
		positionY->setGeometry(QRect(165, 0, 80, 22));
		SpinBoxStyleHelper::applyNormalStyle(positionY);
		connect(positionY, SIGNAL(valueChanged(double)), this, SLOT(updateSoundSpecificProperties()));
		positionZ = new QDoubleSpinBox(pointSoundWidget);
		positionZ->setGeometry(QRect(245, 0, 80, 22));
		SpinBoxStyleHelper::applyNormalStyle(positionZ);
		connect(positionZ, SIGNAL(valueChanged(double)), this, SLOT(updateSoundSpecificProperties()));

		QLabel *inaudibleDistanceLabel= new QLabel(pointSoundWidget);
		inaudibleDistanceLabel->setText("Inaudible Dis.:");
		inaudibleDistanceLabel->setGeometry(QRect(0, 25, 85, 22));

		inaudibleDistance = new QDoubleSpinBox(pointSoundWidget);
		inaudibleDistance->setGeometry(QRect(85, 25, 80, 22));
		SpinBoxStyleHelper::applyNormalStyle(inaudibleDistance);
		inaudibleDistance->setMinimum(0.0);
		connect(inaudibleDistance, SIGNAL(valueChanged(double)), this, SLOT(updateSoundSpecificProperties()));
	}

	void SoundControllerWidget::setupSoundBehaviorPropertiesBox(QWidget *soundTriggerGroupBox)
	{
		QGroupBox *soundBehaviorGroupBox = new QGroupBox(soundTriggerGroupBox);
		soundBehaviorGroupBox->setTitle("Sound Behavior");
		soundBehaviorGroupBox->setGeometry(QRect(5, 15, 345, 95));
		GroupBoxStyleHelper::applyNormalStyle(soundBehaviorGroupBox);

		QLabel *playBehaviorLabel= new QLabel(soundBehaviorGroupBox);
		playBehaviorLabel->setText("Play Behavior:");
		playBehaviorLabel->setGeometry(QRect(5, 15, 95, 22));

		playBehavior = new QComboBox(soundBehaviorGroupBox);
		playBehavior->setGeometry(QRect(110, 15, 130, 22));
		playBehavior->addItem(PLAY_ONCE_LABEL, QVariant(SoundBehavior::PLAY_ONCE));
		playBehavior->addItem(PLAY_LOOP_LABEL, QVariant(SoundBehavior::PLAY_LOOP));
		connect(playBehavior, SIGNAL(currentIndexChanged(int)), this, SLOT(updateSoundBehaviorProperties()));

		QLabel *stopBehaviorLabel= new QLabel(soundBehaviorGroupBox);
		stopBehaviorLabel->setText("Stop Behavior:");
		stopBehaviorLabel->setGeometry(QRect(5, 40, 95, 22));

		stopBehavior = new QComboBox(soundBehaviorGroupBox);
		stopBehavior->setGeometry(QRect(110, 40, 130, 22));
		stopBehavior->addItem(INSTANT_STOP_LABEL, QVariant(SoundBehavior::INSTANT_STOP));
		stopBehavior->addItem(SMOOTH_STOP_LABEL, QVariant(SoundBehavior::SMOOTH_STOP));
		connect(stopBehavior, SIGNAL(currentIndexChanged(int)), this, SLOT(updateSoundBehaviorProperties()));

		QLabel *volumeDecreasePercentageOnStopLabel= new QLabel(soundBehaviorGroupBox);
		volumeDecreasePercentageOnStopLabel->setText("Volume Decrease:");
		volumeDecreasePercentageOnStopLabel->setGeometry(QRect(5, 65, 95, 22));

		volumeDecreasePercentageOnStop = new QDoubleSpinBox(soundBehaviorGroupBox);
		volumeDecreasePercentageOnStop->setGeometry(QRect(110, 65, 80, 22));
		SpinBoxStyleHelper::applyNormalStyle(volumeDecreasePercentageOnStop);
		volumeDecreasePercentageOnStop->setMinimum(0.0);
		volumeDecreasePercentageOnStop->setMaximum(1.0);
		connect(volumeDecreasePercentageOnStop, SIGNAL(valueChanged(double)), this, SLOT(updateSoundBehaviorProperties()));
	}

	void SoundControllerWidget::setupSoundTriggerSpecificPropertiesBox(QWidget *soundTriggerBox)
	{
		QGroupBox *specificPropertiesGroupBox = new QGroupBox(soundTriggerBox);
		specificPropertiesGroupBox->setTitle("Specific Properties");
		specificPropertiesGroupBox->setGeometry(QRect(5, 115, 345, 240));
		GroupBoxStyleHelper::applyNormalStyle(specificPropertiesGroupBox);

		QLabel *soundTriggerTypeLabel= new QLabel(specificPropertiesGroupBox);
		soundTriggerTypeLabel->setText("Trigger Type:");
		soundTriggerTypeLabel->setGeometry(QRect(5, 15, 65, 22));

		soundTriggerType = new QLabel(specificPropertiesGroupBox);
		soundTriggerType->setGeometry(QRect(85, 15, 80, 22));

		changeSoundTriggerTypeButton = new QPushButton(specificPropertiesGroupBox);
		changeSoundTriggerTypeButton->setText("Change Trigger");
		changeSoundTriggerTypeButton->setGeometry(QRect(180, 15, 85, 22));
		connect(changeSoundTriggerTypeButton, SIGNAL(clicked()), this, SLOT(showChangeSoundTriggerDialog()));

		setupManuelTriggerBox(specificPropertiesGroupBox);
		setupShapeTriggerBox(specificPropertiesGroupBox);
	}

	void SoundControllerWidget::setupManuelTriggerBox(QWidget *specificPropertiesGroupBox)
	{
		manualTriggerWidget = new QWidget(specificPropertiesGroupBox);
		manualTriggerWidget->setGeometry(QRect(5, 40, 345, 235));
		manualTriggerWidget->hide();
	}

	void SoundControllerWidget::setupShapeTriggerBox(QWidget *specificPropertiesGroupBox)
	{
		shapeTriggerWidget = new QWidget(specificPropertiesGroupBox);
		shapeTriggerWidget->setGeometry(QRect(5, 40, 345, 235));
		shapeTriggerWidget->hide();

		QLabel *soundShapeTypeLabel= new QLabel(shapeTriggerWidget);
		soundShapeTypeLabel->setText("Shape Type:");
		soundShapeTypeLabel->setGeometry(QRect(0, 0, 65, 22));

		soundShapeType = new QLabel(shapeTriggerWidget);
		soundShapeType->setGeometry(QRect(80, 0, 80, 22));

		changeSoundShapeTypeButton = new QPushButton(shapeTriggerWidget);
		changeSoundShapeTypeButton->setText("Change Shape");
		changeSoundShapeTypeButton->setGeometry(QRect(175, 0, 85, 22));
		connect(changeSoundShapeTypeButton, SIGNAL(clicked()), this, SLOT(showChangeSoundShapeDialog()));

		soundShapeWidget = nullptr;
	}

	SoundTableView *SoundControllerWidget::getSoundTableView() const
	{
		return soundTableView;
	}

	void SoundControllerWidget::load(SoundController *soundController)
	{
		this->soundController = soundController;

		std::list<const SceneSound *> sceneSounds = soundController->getSceneSounds();
		for(std::list<const SceneSound *>::const_iterator it=sceneSounds.begin(); it!=sceneSounds.end(); ++it)
		{
			soundTableView->addSound((*it));
		}
	}

	void SoundControllerWidget::unload()
	{
		soundTableView->removeAllSounds();

		soundController = nullptr;
	}

	void SoundControllerWidget::notify(Observable *observable, int notificationType)
	{
		if(SoundTableView *soundTableView = dynamic_cast<SoundTableView *>(observable))
		{
			switch(notificationType)
			{
				case SoundTableView::SELECTION_CHANGED:
					if(soundTableView->hasSceneSoundSelected())
					{
						const SceneSound *sceneSound = soundTableView->getSelectedSceneSound();
						setupSoundDataFrom(sceneSound);

						removeSoundButton->setEnabled(true);
						soundGroupBox->show();
						soundTriggerGroupBox->show();
					}else
					{
						removeSoundButton->setEnabled(false);
						soundGroupBox->hide();
						soundTriggerGroupBox->hide();
					}
					break;
			}
		}
	}

	void SoundControllerWidget::setupSoundDataFrom(const SceneSound *sceneSound)
	{
		disableSoundEvent = true;

		//sound
		const Sound *sound = sceneSound->getSound();

		this->volume->setValue(sound->getVolume());

		if(sound->getSoundType()==Sound::SoundType::AMBIENT)
		{
			setupAmbientSoundDataFrom(static_cast<const AmbientSound *>(sound));
		}else if(sound->getSoundType()==Sound::SoundType::POINT)
		{
			setupPointSoundDataFrom(static_cast<const PointSound *>(sound));
		}else
		{
			throw std::invalid_argument("Impossible to setup specific sound data for sound of type: " + sound->getSoundType());
		}

		//sound trigger
		const SoundTrigger *soundTrigger = sceneSound->getSoundTrigger();

		setupSoundBehaviorDataFrom(soundTrigger);

		if(soundTrigger->getTriggerType()==SoundTrigger::TriggerType::MANUAL_TRIGGER)
		{
			setupManualTriggerDataFrom(sceneSound);
		}else if(soundTrigger->getTriggerType()==SoundTrigger::TriggerType::SHAPE_TRIGGER)
		{
			setupShapeTriggerDataFrom(sceneSound);
		}else
		{
			throw std::invalid_argument("Impossible to setup specific sound trigger data for sound trigger of type: " + soundTrigger->getTriggerType());
		}

		disableSoundEvent = false;
	}

	void SoundControllerWidget::setupAmbientSoundDataFrom(const AmbientSound *)
	{
		ambientSoundWidget->show();
		pointSoundWidget->hide();

		soundType->setText(AMBIENT_SOUND_LABEL);
	}

	void SoundControllerWidget::setupPointSoundDataFrom(const PointSound *pointSound)
	{
		pointSoundWidget->show();
		ambientSoundWidget->hide();

		soundType->setText(POINT_SOUND_LABEL);

		this->positionX->setValue(pointSound->getPosition().X);
		this->positionY->setValue(pointSound->getPosition().Y);
		this->positionZ->setValue(pointSound->getPosition().Z);

		this->inaudibleDistance->setValue(pointSound->getInaudibleDistance());
	}

	void SoundControllerWidget::setupSoundBehaviorDataFrom(const SoundTrigger *soundTrigger)
	{
		const SoundBehavior &soundBehavior = soundTrigger->getSoundBehavior();

		int playBehaviorIndex = playBehavior->findData(soundBehavior.getPlayBehavior());
		if(playBehaviorIndex!=-1)
		{
			playBehavior->setCurrentIndex(playBehaviorIndex);
		}

		int stopBehaviorIndex = stopBehavior->findData(soundBehavior.getStopBehavior());
		if(stopBehaviorIndex!=-1)
		{
			stopBehavior->setCurrentIndex(stopBehaviorIndex);
		}

		this->volumeDecreasePercentageOnStop->setValue(soundBehavior.getVolumeDecreasePercentageOnStop());
	}

	void SoundControllerWidget::setupManualTriggerDataFrom(const SceneSound *)
	{
		manualTriggerWidget->show();
		shapeTriggerWidget->hide();

		soundTriggerType->setText(MANUAL_TRIGGER_LABEL);
	}

	void SoundControllerWidget::setupShapeTriggerDataFrom(const SceneSound *sceneSound)
	{
		shapeTriggerWidget->show();
		manualTriggerWidget->hide();

		soundTriggerType->setText(SHAPE_TRIGGER_LABEL);

		const ShapeTrigger *shapeTrigger = static_cast<const ShapeTrigger *>(sceneSound->getSoundTrigger());
		const SoundShape *soundShape = shapeTrigger->getSoundShape();
		SoundShapeWidget *soundShapeWidget = retrieveSoundShapeWidget(soundShape, sceneSound);
		soundShapeWidget->setupShapePropertiesFrom(soundShape);

		soundShapeType->setText(QString::fromStdString(soundShapeWidget->getSoundShapeName()));
	}

	SoundShapeWidget *SoundControllerWidget::retrieveSoundShapeWidget(const SoundShape *shape, const SceneSound *sceneSound)
	{
		delete soundShapeWidget;
		soundShapeWidget = SoundShapeWidgetRetriever(shapeTriggerWidget, sceneSound).retrieveShapeWidget(shape);
		connect(soundShapeWidget, SIGNAL(soundShapeChange(const SoundShape *)), this, SLOT(soundShapeChanged(const SoundShape *)));

		soundShapeWidget->setGeometry(QRect(0, 30, 339, 230));
		soundShapeWidget->show();
		return soundShapeWidget;
	}

	void SoundControllerWidget::showAddSoundDialog()
	{
		NewSoundDialog newSceneSoundDialog(this, soundController);
		newSceneSoundDialog.exec();

		if(newSceneSoundDialog.result()==QDialog::Accepted)
		{
			SceneSound *sceneSound = newSceneSoundDialog.getSceneSound();
			soundController->addSceneSound(sceneSound);

			soundTableView->addSound(sceneSound);
		}
	}

	void SoundControllerWidget::removeSelectedSound()
	{
		if(soundTableView->hasSceneSoundSelected())
		{
			const SceneSound *sceneSound = soundTableView->getSelectedSceneSound();
			soundController->removeSceneSound(sceneSound);

			soundTableView->removeSelectedSound();
		}
	}

	void SoundControllerWidget::updateSoundGeneralProperties()
	{
		if(!disableSoundEvent)
		{
			const SceneSound *sceneSound = soundTableView->getSelectedSceneSound();

			float volume = this->volume->value();

			soundController->updateSceneSoundGeneralProperties(sceneSound, volume);
		}
	}

	void SoundControllerWidget::updateSoundSpecificProperties()
	{
		if(!disableSoundEvent)
		{
			const SceneSound *sceneSound = soundTableView->getSelectedSceneSound();
			const Sound *sound = sceneSound->getSound();

			if(sound->getSoundType()==Sound::AMBIENT)
			{
				//nothing to update
			}else if(sound->getSoundType()==Sound::POINT)
			{
				Point3<float> position(positionX->value(), positionY->value(), positionY->value());
				float inaudibleDistance = this->inaudibleDistance->value();

				soundController->updateScenePointSoundProperties(sceneSound, position, inaudibleDistance);
			}else
			{
				throw std::invalid_argument("Unknown sound type to update specific properties: " + sound->getSoundType());
			}
		}
	}

	void SoundControllerWidget::updateSoundBehaviorProperties()
	{
		if(!disableSoundEvent)
		{
			const SceneSound *sceneSound = soundTableView->getSelectedSceneSound();

			QVariant playVariant = playBehavior->currentData();
			SoundBehavior::PlayBehavior playBehavior = static_cast<SoundBehavior::PlayBehavior>(playVariant.toInt());

			QVariant stopVariant = stopBehavior->currentData();
			SoundBehavior::StopBehavior stopBehavior = static_cast<SoundBehavior::StopBehavior>(stopVariant.toInt());

			float volumeDecreasePercentageOnStop = this->volumeDecreasePercentageOnStop->value();

			soundController->updateSceneSoundBehaviorProperties(sceneSound, playBehavior, stopBehavior, volumeDecreasePercentageOnStop);
		}
	}

	void SoundControllerWidget::showChangeSoundTriggerDialog()
	{
		ChangeSoundTriggerDialog changeSoundTriggerDialog(this);
		changeSoundTriggerDialog.exec();

		if(changeSoundTriggerDialog.result()==QDialog::Accepted)
		{
			const SceneSound *sceneSound = soundTableView->getSelectedSceneSound();
			SoundTrigger::TriggerType triggerType = changeSoundTriggerDialog.getTriggerType();

			soundController->changeSoundTrigger(sceneSound, triggerType);
			setupSoundDataFrom(sceneSound);
		}
	}

	void SoundControllerWidget::showChangeSoundShapeDialog()
	{
		ChangeSoundShapeDialog changeSoundShapeDialog(this);
		changeSoundShapeDialog.exec();

		if(changeSoundShapeDialog.result()==QDialog::Accepted)
		{
			const SceneSound *sceneSound = soundTableView->getSelectedSceneSound();
			SoundShape::ShapeType shapeType = changeSoundShapeDialog.getShapeType();

			soundController->changeSoundShape(sceneSound, shapeType);
			setupShapeTriggerDataFrom(sceneSound);
		}
	}

	void SoundControllerWidget::soundShapeChanged(const SoundShape *soundShape)
	{
		if(!disableSoundEvent)
		{
			const SceneSound *sceneSound = soundTableView->getSelectedSceneSound();

			soundController->updateSceneSoundShape(sceneSound, soundShape);
		}
	}
}
