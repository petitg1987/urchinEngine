#include <QtWidgets/QHBoxLayout>

#include "SoundControllerWidget.h"
#include "support/GroupBoxStyleHelper.h"
#include "support/SpinBoxStyleHelper.h"
#include "support/ButtonStyleHelper.h"
#include "support/FrameStyleHelper.h"
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
		QVBoxLayout *mainLayout = new QVBoxLayout(this);
		mainLayout->setAlignment(Qt::AlignTop);
		mainLayout->setContentsMargins(1, 1, 1, 1);

		soundTableView = new SoundTableView();
		mainLayout->addWidget(soundTableView);
		soundTableView->addObserver(this, SoundTableView::SELECTION_CHANGED);
		soundTableView->setFixedHeight(220);

		QHBoxLayout *buttonsLayout = new QHBoxLayout();
		mainLayout->addLayout(buttonsLayout);
		buttonsLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

		addSoundButton = new QPushButton("New Sound");
		buttonsLayout->addWidget(addSoundButton);
		ButtonStyleHelper::applyNormalStyle(addSoundButton);
		connect(addSoundButton, SIGNAL(clicked()), this, SLOT(showAddSoundDialog()));

		removeSoundButton = new QPushButton("Remove Sound");
		buttonsLayout->addWidget(removeSoundButton);
		ButtonStyleHelper::applyNormalStyle(removeSoundButton);
		removeSoundButton->setEnabled(false);
		connect(removeSoundButton, SIGNAL(clicked()), this, SLOT(removeSelectedSound()));

		soundPropertiesGroupBox = new QGroupBox("Sound Properties");
		mainLayout->addWidget(soundPropertiesGroupBox);
		GroupBoxStyleHelper::applyNormalStyle(soundPropertiesGroupBox);
		soundPropertiesGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
		soundPropertiesGroupBox->hide();

		QVBoxLayout *soundPropertiesLayout = new QVBoxLayout(soundPropertiesGroupBox);
		setupSoundGeneralPropertiesBox(soundPropertiesLayout);
		setupSpecificPointSoundBox(soundPropertiesLayout);

		soundTriggerGroupBox = new QGroupBox("Sound Trigger");
		mainLayout->addWidget(soundTriggerGroupBox);
		GroupBoxStyleHelper::applyNormalStyle(soundTriggerGroupBox);
		soundTriggerGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
		soundTriggerGroupBox->hide();

		QVBoxLayout *soundTriggerLayout = new QVBoxLayout(soundTriggerGroupBox);
		setupSoundBehaviorPropertiesBox(soundTriggerLayout);
		setupSpecificTriggerShapeBox(soundTriggerLayout);
	}

	SoundControllerWidget::~SoundControllerWidget()
	{

	}

	void SoundControllerWidget::setupSoundGeneralPropertiesBox(QVBoxLayout *soundPropertiesLayout)
	{
		QGroupBox *generalGroupBox = new QGroupBox("General");
		soundPropertiesLayout->addWidget(generalGroupBox);
		GroupBoxStyleHelper::applyNormalStyle(generalGroupBox);

		QGridLayout *generalPropertiesLayout = new QGridLayout(generalGroupBox);
		generalPropertiesLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

		QLabel *volumeLabel= new QLabel("Volume:");
		generalPropertiesLayout->addWidget(volumeLabel, 0, 0);

		volume = new QDoubleSpinBox();
		generalPropertiesLayout->addWidget(volume, 0, 1);
		SpinBoxStyleHelper::applyDefaultStyleOn(volume);
		volume->setMinimum(0.0);
		connect(volume, SIGNAL(valueChanged(double)), this, SLOT(updateSoundGeneralProperties()));

		QLabel *soundTypeLabel= new QLabel("Sound Type:");
		generalPropertiesLayout->addWidget(soundTypeLabel, 1, 0);

		soundType = new QLabel();
		generalPropertiesLayout->addWidget(soundType, 1, 1);
	}

	void SoundControllerWidget::setupSpecificPointSoundBox(QVBoxLayout *soundPropertiesLayout)
	{
		specificPointSoundGroupBox = new QGroupBox("Point Sound");
		soundPropertiesLayout->addWidget(specificPointSoundGroupBox);
		GroupBoxStyleHelper::applyNormalStyle(specificPointSoundGroupBox);
		specificPointSoundGroupBox->hide();

		QGridLayout *pointSoundLayout = new QGridLayout(specificPointSoundGroupBox);
		pointSoundLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

		QLabel *positionLabel= new QLabel("Position:");
		pointSoundLayout->addWidget(positionLabel, 0, 0);

		QHBoxLayout *positionLayout = new QHBoxLayout();
		pointSoundLayout->addLayout(positionLayout, 0, 1);
		positionX = new QDoubleSpinBox();
		positionLayout->addWidget(positionX);
		SpinBoxStyleHelper::applyDefaultStyleOn(positionX);
		connect(positionX, SIGNAL(valueChanged(double)), this, SLOT(updateSoundSpecificProperties()));
		positionY = new QDoubleSpinBox();
		positionLayout->addWidget(positionY);
		SpinBoxStyleHelper::applyDefaultStyleOn(positionY);
		connect(positionY, SIGNAL(valueChanged(double)), this, SLOT(updateSoundSpecificProperties()));
		positionZ = new QDoubleSpinBox();
		positionLayout->addWidget(positionZ);
		SpinBoxStyleHelper::applyDefaultStyleOn(positionZ);
		connect(positionZ, SIGNAL(valueChanged(double)), this, SLOT(updateSoundSpecificProperties()));

		QLabel *inaudibleDistanceLabel= new QLabel("Inaudible\nDistance:");
		pointSoundLayout->addWidget(inaudibleDistanceLabel, 1, 0);

		inaudibleDistance = new QDoubleSpinBox();
		pointSoundLayout->addWidget(inaudibleDistance, 1, 1);
		SpinBoxStyleHelper::applyDefaultStyleOn(inaudibleDistance);
		inaudibleDistance->setMinimum(0.0);
		connect(inaudibleDistance, SIGNAL(valueChanged(double)), this, SLOT(updateSoundSpecificProperties()));
	}

	void SoundControllerWidget::setupSoundBehaviorPropertiesBox(QVBoxLayout *soundTriggerLayout)
	{
		QGroupBox *soundBehaviorGroupBox = new QGroupBox("Sound Behavior");
		soundTriggerLayout->addWidget(soundBehaviorGroupBox);
		GroupBoxStyleHelper::applyNormalStyle(soundBehaviorGroupBox);

		QGridLayout *behaviorLayout = new QGridLayout(soundBehaviorGroupBox);
		behaviorLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

		QLabel *playBehaviorLabel= new QLabel("Play Behavior:");
		behaviorLayout->addWidget(playBehaviorLabel, 0, 0);

		playBehavior = new QComboBox();
		behaviorLayout->addWidget(playBehavior, 0, 1, 1, 3);
		playBehavior->addItem(PLAY_ONCE_LABEL, QVariant(SoundBehavior::PLAY_ONCE));
		playBehavior->addItem(PLAY_LOOP_LABEL, QVariant(SoundBehavior::PLAY_LOOP));
		connect(playBehavior, SIGNAL(currentIndexChanged(int)), this, SLOT(updateSoundBehaviorProperties()));

		QLabel *stopBehaviorLabel= new QLabel("Stop Behavior:");
		behaviorLayout->addWidget(stopBehaviorLabel, 1, 0);

		stopBehavior = new QComboBox();
		behaviorLayout->addWidget(stopBehavior, 1, 1, 1, 3);
		stopBehavior->addItem(INSTANT_STOP_LABEL, QVariant(SoundBehavior::INSTANT_STOP));
		stopBehavior->addItem(SMOOTH_STOP_LABEL, QVariant(SoundBehavior::SMOOTH_STOP));
		connect(stopBehavior, SIGNAL(currentIndexChanged(int)), this, SLOT(updateSoundBehaviorProperties()));

		QLabel *volumeDecreasePercentageOnStopLabel= new QLabel("Vol. Decrease (%)\nper second:");
		behaviorLayout->addWidget(volumeDecreasePercentageOnStopLabel, 2, 0);

		volumeDecreasePercentageOnStop = new QDoubleSpinBox();
		behaviorLayout->addWidget(volumeDecreasePercentageOnStop, 2, 1, 1, 3);
		SpinBoxStyleHelper::applyPercentageStyleOn(volumeDecreasePercentageOnStop);
		connect(volumeDecreasePercentageOnStop, SIGNAL(valueChanged(double)), this, SLOT(updateSoundBehaviorProperties()));

		QLabel *soundTriggerTypeLabel= new QLabel("Trigger:");
		behaviorLayout->addWidget(soundTriggerTypeLabel, 3, 0);

		soundTriggerType = new QLabel();
		behaviorLayout->addWidget(soundTriggerType, 3, 1);

		changeSoundTriggerTypeButton = new QPushButton("Change");
		behaviorLayout->addWidget(changeSoundTriggerTypeButton, 3, 2);
		ButtonStyleHelper::applyNormalStyle(changeSoundTriggerTypeButton);
		connect(changeSoundTriggerTypeButton, SIGNAL(clicked()), this, SLOT(showChangeSoundTriggerDialog()));
	}

	void SoundControllerWidget::setupSpecificTriggerShapeBox(QVBoxLayout *soundTriggerLayout)
	{
		specificTriggerShapeGroupBox = new QGroupBox("Trigger Shape");
		soundTriggerLayout->addWidget(specificTriggerShapeGroupBox);
		GroupBoxStyleHelper::applyNormalStyle(specificTriggerShapeGroupBox);
		specificTriggerShapeGroupBox->hide();

		triggerShapeLayout = new QVBoxLayout(specificTriggerShapeGroupBox);

		QHBoxLayout *shapeTypeLayout = new QHBoxLayout();
		shapeTypeLayout->setAlignment(Qt::AlignLeft);
		shapeTypeLayout->setSpacing(15);
		triggerShapeLayout->addLayout(shapeTypeLayout);

		QLabel *soundShapeTypeLabel= new QLabel("Shape:");
		shapeTypeLayout->addWidget(soundShapeTypeLabel);

		soundShapeType = new QLabel();
		shapeTypeLayout->addWidget(soundShapeType);

		changeSoundShapeTypeButton = new QPushButton("Change");
		shapeTypeLayout->addWidget(changeSoundShapeTypeButton);
		ButtonStyleHelper::applyNormalStyle(changeSoundShapeTypeButton);
		connect(changeSoundShapeTypeButton, SIGNAL(clicked()), this, SLOT(showChangeSoundShapeDialog()));

		QFrame *frameLine = new QFrame();
		triggerShapeLayout->addWidget(frameLine);
		FrameStyleHelper::applyLineStyle(frameLine);

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
						soundPropertiesGroupBox->show();
						soundTriggerGroupBox->show();
					}else
					{
						removeSoundButton->setEnabled(false);
						soundPropertiesGroupBox->hide();
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
			throw std::invalid_argument("Impossible to setup specific sound data for sound of type: " + std::to_string(sound->getSoundType()));
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
			throw std::invalid_argument("Impossible to setup specific sound trigger data for sound trigger of type: " + std::to_string(soundTrigger->getTriggerType()));
		}

		disableSoundEvent = false;
	}

	void SoundControllerWidget::setupAmbientSoundDataFrom(const AmbientSound *)
	{
		specificPointSoundGroupBox->hide();

		soundType->setText(AMBIENT_SOUND_LABEL);
	}

	void SoundControllerWidget::setupPointSoundDataFrom(const PointSound *pointSound)
	{
		specificPointSoundGroupBox->show();

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

		this->volumeDecreasePercentageOnStop->setValue(soundBehavior.getVolumeDecreasePercentageOnStop() * 100.0);
	}

	void SoundControllerWidget::setupManualTriggerDataFrom(const SceneSound *)
	{
		specificTriggerShapeGroupBox->hide();
		soundTriggerType->setText(MANUAL_TRIGGER_LABEL);
	}

	void SoundControllerWidget::setupShapeTriggerDataFrom(const SceneSound *sceneSound)
	{
		specificTriggerShapeGroupBox->show();
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

		soundShapeWidget = SoundShapeWidgetRetriever(sceneSound).retrieveShapeWidget(shape);
		triggerShapeLayout->addWidget(soundShapeWidget);
		soundShapeWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
		soundShapeWidget->show();
		connect(soundShapeWidget, SIGNAL(soundShapeChange(const SoundShape *)), this, SLOT(soundShapeChanged(const SoundShape *)));

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
				throw std::invalid_argument("Unknown sound type to update specific properties: " + std::to_string(sound->getSoundType()));
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

			float volumeDecreasePercentageOnStop = this->volumeDecreasePercentageOnStop->value() / 100.0;

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
