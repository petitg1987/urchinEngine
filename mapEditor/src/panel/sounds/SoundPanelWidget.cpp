#include <QtWidgets/QHBoxLayout>

#include <panel/sounds/SoundPanelWidget.h>
#include <widget/style/GroupBoxStyleHelper.h>
#include <widget/style/SpinBoxStyleHelper.h>
#include <widget/style/ButtonStyleHelper.h>
#include <widget/style/FrameStyleHelper.h>
#include <panel/sounds/dialog/NewSoundDialog.h>
#include <panel/sounds/dialog/ChangeSoundTriggerDialog.h>
#include <panel/sounds/dialog/ChangeSoundShapeDialog.h>
#include <panel/sounds/soundshape/SoundShapeWidgetRetriever.h>

namespace urchin {

    SoundPanelWidget::SoundPanelWidget() :
            soundController(nullptr),
            soundTableView(nullptr),
            addSoundButton(nullptr),
            removeSoundButton(nullptr),
            soundPropertiesGroupBox(nullptr),
            soundTriggerGroupBox(nullptr),
            specificTriggerShapeGroupBox(nullptr),
            specificSpatialSoundGroupBox(nullptr),
            disableSoundEvent(false),
            initialVolume(nullptr),
            soundType(nullptr),
            soundCategory(nullptr),
            positionX(nullptr), positionY(nullptr), positionZ(nullptr), inaudibleDistance(nullptr),
            playBehavior(nullptr),
            soundTriggerType(nullptr),
            triggerShapeLayout(nullptr),
            changeSoundTriggerTypeButton(nullptr),
            soundShapeType(nullptr),
            changeSoundShapeTypeButton(nullptr),
            soundShapeWidget(nullptr) {
        auto* mainLayout = new QVBoxLayout(this);
        mainLayout->setAlignment(Qt::AlignTop);
        mainLayout->setContentsMargins(1, 1, 1, 1);

        soundTableView = new SoundTableView();
        mainLayout->addWidget(soundTableView);
        soundTableView->addObserver(this, SoundTableView::SOUND_SELECTION_CHANGED);
        soundTableView->setFixedHeight(220);

        auto* buttonsLayout = new QHBoxLayout();
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

        auto* soundPropertiesLayout = new QVBoxLayout(soundPropertiesGroupBox);
        setupSoundGeneralPropertiesBox(soundPropertiesLayout);
        setupSpecificSpatialSoundBox(soundPropertiesLayout);

        soundTriggerGroupBox = new QGroupBox("Sound Trigger");
        mainLayout->addWidget(soundTriggerGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(soundTriggerGroupBox);
        soundTriggerGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        soundTriggerGroupBox->hide();

        auto* soundTriggerLayout = new QVBoxLayout(soundTriggerGroupBox);
        setupSoundTriggerGeneralPropertiesBox(soundTriggerLayout);
        setupSpecificTriggerShapeBox(soundTriggerLayout);
    }

    void SoundPanelWidget::setupSoundGeneralPropertiesBox(QVBoxLayout* soundPropertiesLayout) {
        auto* generalGroupBox = new QGroupBox("General");
        soundPropertiesLayout->addWidget(generalGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(generalGroupBox);

        auto* generalPropertiesLayout = new QGridLayout(generalGroupBox);
        generalPropertiesLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        auto* initialVolumeLabel= new QLabel("Initial volume:");
        generalPropertiesLayout->addWidget(initialVolumeLabel, 0, 0);

        initialVolume = new QDoubleSpinBox();
        generalPropertiesLayout->addWidget(initialVolume, 0, 1);
        SpinBoxStyleHelper::applyDefaultStyleOn(initialVolume);
        initialVolume->setMinimum(0.0);
        connect(initialVolume, SIGNAL(valueChanged(double)), this, SLOT(updateSoundGeneralProperties()));

        auto *soundTypeLabel= new QLabel("Sound Type:");
        generalPropertiesLayout->addWidget(soundTypeLabel, 1, 0);
        soundType = new QLabel();
        generalPropertiesLayout->addWidget(soundType, 1, 1);

        auto *soundCategoryLabel= new QLabel("Sound Category:");
        generalPropertiesLayout->addWidget(soundCategoryLabel, 2, 0);
        soundCategory = new QLabel();
        generalPropertiesLayout->addWidget(soundCategory, 2, 1);
    }

    void SoundPanelWidget::setupSpecificSpatialSoundBox(QVBoxLayout* soundPropertiesLayout) {
        specificSpatialSoundGroupBox = new QGroupBox("Spatial Sound");
        soundPropertiesLayout->addWidget(specificSpatialSoundGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(specificSpatialSoundGroupBox);
        specificSpatialSoundGroupBox->hide();

        auto* spatialSoundLayout = new QGridLayout(specificSpatialSoundGroupBox);
        spatialSoundLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        auto *positionLabel= new QLabel("Position:");
        spatialSoundLayout->addWidget(positionLabel, 0, 0);

        auto* positionLayout = new QHBoxLayout();
        spatialSoundLayout->addLayout(positionLayout, 0, 1);
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

        auto *inaudibleDistanceLabel= new QLabel("Inaudible\nDistance:");
        spatialSoundLayout->addWidget(inaudibleDistanceLabel, 1, 0);

        inaudibleDistance = new QDoubleSpinBox();
        spatialSoundLayout->addWidget(inaudibleDistance, 1, 1);
        SpinBoxStyleHelper::applyDefaultStyleOn(inaudibleDistance);
        inaudibleDistance->setMinimum(0.0);
        connect(inaudibleDistance, SIGNAL(valueChanged(double)), this, SLOT(updateSoundSpecificProperties()));
    }

    void SoundPanelWidget::setupSoundTriggerGeneralPropertiesBox(QVBoxLayout* soundTriggerLayout) {
        auto* generalGroupBox = new QGroupBox("General");
        soundTriggerLayout->addWidget(generalGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(generalGroupBox);

        auto* generalLayout = new QGridLayout(generalGroupBox);
        generalLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        auto *playBehaviorLabel= new QLabel("Play Behavior:");
        generalLayout->addWidget(playBehaviorLabel, 0, 0);

        playBehavior = new QComboBox();
        generalLayout->addWidget(playBehavior, 0, 1, 1, 3);
        playBehavior->addItem(PLAY_ONCE_LABEL, QVariant(SoundTrigger::PLAY_ONCE));
        playBehavior->addItem(PLAY_LOOP_LABEL, QVariant(SoundTrigger::PLAY_LOOP));
        connect(playBehavior, SIGNAL(currentIndexChanged(int)), this, SLOT(updateSoundTriggerProperties()));

        auto *soundTriggerTypeLabel= new QLabel("Trigger:");
        generalLayout->addWidget(soundTriggerTypeLabel, 3, 0);

        soundTriggerType = new QLabel();
        generalLayout->addWidget(soundTriggerType, 3, 1);

        changeSoundTriggerTypeButton = new QPushButton("Change");
        generalLayout->addWidget(changeSoundTriggerTypeButton, 3, 2);
        ButtonStyleHelper::applyNormalStyle(changeSoundTriggerTypeButton);
        connect(changeSoundTriggerTypeButton, SIGNAL(clicked()), this, SLOT(showChangeSoundTriggerDialog()));
    }

    void SoundPanelWidget::setupSpecificTriggerShapeBox(QVBoxLayout* soundTriggerLayout) {
        specificTriggerShapeGroupBox = new QGroupBox("Trigger Shape");
        soundTriggerLayout->addWidget(specificTriggerShapeGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(specificTriggerShapeGroupBox);
        specificTriggerShapeGroupBox->hide();

        triggerShapeLayout = new QVBoxLayout(specificTriggerShapeGroupBox);

        auto* shapeTypeLayout = new QHBoxLayout();
        shapeTypeLayout->setAlignment(Qt::AlignLeft);
        shapeTypeLayout->setSpacing(15);
        triggerShapeLayout->addLayout(shapeTypeLayout);

        auto *soundShapeTypeLabel= new QLabel("Shape:");
        shapeTypeLayout->addWidget(soundShapeTypeLabel);

        soundShapeType = new QLabel();
        shapeTypeLayout->addWidget(soundShapeType);

        changeSoundShapeTypeButton = new QPushButton("Change");
        shapeTypeLayout->addWidget(changeSoundShapeTypeButton);
        ButtonStyleHelper::applyNormalStyle(changeSoundShapeTypeButton);
        connect(changeSoundShapeTypeButton, SIGNAL(clicked()), this, SLOT(showChangeSoundShapeDialog()));

        auto* frameLine = new QFrame();
        triggerShapeLayout->addWidget(frameLine);
        FrameStyleHelper::applyLineStyle(frameLine);

        soundShapeWidget = nullptr;
    }

    SoundTableView* SoundPanelWidget::getSoundTableView() const {
        return soundTableView;
    }

    void SoundPanelWidget::load(SoundController* soundController) {
        this->soundController = soundController;

        std::list<const SceneSound*> sceneSounds = soundController->getSceneSounds();
        for (auto& sceneSound : sceneSounds) {
            soundTableView->addSound(sceneSound);
        }
    }

    void SoundPanelWidget::unload() {
        soundTableView->removeAllSounds();

        soundController = nullptr;
    }

    void SoundPanelWidget::notify(Observable* observable, int notificationType) {
        if (auto* soundTableView = dynamic_cast<SoundTableView*>(observable)) {
            if (notificationType == SoundTableView::SOUND_SELECTION_CHANGED) {
                if (soundTableView->hasSceneSoundSelected()) {
                    const SceneSound* sceneSound = soundTableView->getSelectedSceneSound();
                    setupSoundDataFrom(sceneSound);

                    removeSoundButton->setEnabled(true);
                    soundPropertiesGroupBox->show();
                    soundTriggerGroupBox->show();
                } else {
                    removeSoundButton->setEnabled(false);
                    soundPropertiesGroupBox->hide();
                    soundTriggerGroupBox->hide();
                }
            }
        }
    }

    void SoundPanelWidget::setupSoundDataFrom(const SceneSound* sceneSound) {
        disableSoundEvent = true;

        //sound
        const Sound* sound = sceneSound->getSound();

        this->initialVolume->setValue(sound->getInitialVolume());

        if (sound->getSoundType() == Sound::SoundType::GLOBAL) {
            setupGlobalSoundDataFrom();
        } else if (sound->getSoundType() == Sound::SoundType::SPATIAL) {
            setupSpatialSoundDataFrom(dynamic_cast<const SpatialSound*>(sound));
        } else {
            throw std::invalid_argument("Impossible to setup specific sound data for sound of type: " + std::to_string(sound->getSoundType()));
        }

        if (sound->getSoundCategory() == Sound::SoundCategory::MUSIC) {
            soundCategory->setText(NewSoundDialog::MUSIC_SOUND_LABEL);
        } else if (sound->getSoundCategory() == Sound::SoundCategory::EFFECTS) {
            soundCategory->setText(NewSoundDialog::EFFECTS_SOUND_LABEL);
        } else {
            throw std::invalid_argument("Impossible to setup specific sound data for sound of category: " + std::to_string(sound->getSoundCategory()));
        }

        //sound trigger
        const SoundTrigger* soundTrigger = sceneSound->getSoundTrigger();
        setupPlayBehaviorDataFrom(soundTrigger);

        if (soundTrigger->getTriggerType() == SoundTrigger::TriggerType::MANUAL_TRIGGER) {
            setupManualTriggerDataFrom();
        } else if (soundTrigger->getTriggerType() == SoundTrigger::TriggerType::SHAPE_TRIGGER) {
            setupShapeTriggerDataFrom(sceneSound);
        } else {
            throw std::invalid_argument("Impossible to setup specific sound trigger data for sound trigger of type: " + std::to_string(soundTrigger->getTriggerType()));
        }

        disableSoundEvent = false;
    }

    void SoundPanelWidget::setupGlobalSoundDataFrom() {
        specificSpatialSoundGroupBox->hide();

        soundType->setText(NewSoundDialog::GLOBAL_SOUND_LABEL);
    }

    void SoundPanelWidget::setupSpatialSoundDataFrom(const SpatialSound* spatialSound) {
        specificSpatialSoundGroupBox->show();

        soundType->setText(NewSoundDialog::SPATIAL_SOUND_LABEL);

        this->positionX->setValue(spatialSound->getPosition().X);
        this->positionY->setValue(spatialSound->getPosition().Y);
        this->positionZ->setValue(spatialSound->getPosition().Z);

        this->inaudibleDistance->setValue(spatialSound->getInaudibleDistance());
    }

    void SoundPanelWidget::setupPlayBehaviorDataFrom(const SoundTrigger* soundTrigger) {
        int playBehaviorIndex = playBehavior->findData(soundTrigger->getPlayBehavior());
        if (playBehaviorIndex != -1) {
            playBehavior->setCurrentIndex(playBehaviorIndex);
        }
    }

    void SoundPanelWidget::setupManualTriggerDataFrom() {
        specificTriggerShapeGroupBox->hide();
        soundTriggerType->setText(ChangeSoundTriggerDialog::MANUAL_TRIGGER_LABEL);
    }

    void SoundPanelWidget::setupShapeTriggerDataFrom(const SceneSound* sceneSound) {
        specificTriggerShapeGroupBox->show();
        soundTriggerType->setText(ChangeSoundTriggerDialog::SHAPE_TRIGGER_LABEL);

        const auto* shapeTrigger = dynamic_cast<const ShapeTrigger*>(sceneSound->getSoundTrigger());
        const SoundShape* soundShape = shapeTrigger->getSoundShape();
        SoundShapeWidget* soundShapeWidget = retrieveSoundShapeWidget(soundShape, sceneSound);
        soundShapeWidget->setupShapePropertiesFrom(soundShape);

        soundShapeType->setText(QString::fromStdString(soundShapeWidget->getSoundShapeName()));
    }

    SoundShapeWidget* SoundPanelWidget::retrieveSoundShapeWidget(const SoundShape* shape, const SceneSound* sceneSound) {
        delete soundShapeWidget;

        soundShapeWidget = SoundShapeWidgetRetriever(sceneSound).retrieveShapeWidget(shape);
        triggerShapeLayout->addWidget(soundShapeWidget);
        soundShapeWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        soundShapeWidget->show();
        connect(soundShapeWidget, SIGNAL(soundShapeChange(const SoundShape*)), this, SLOT(soundShapeChanged(const SoundShape*)));

        return soundShapeWidget;
    }

    void SoundPanelWidget::showAddSoundDialog() {
        NewSoundDialog newSceneSoundDialog(this, soundController);
        newSceneSoundDialog.exec();

        if (newSceneSoundDialog.result() == QDialog::Accepted) {
            SceneSound* sceneSound = newSceneSoundDialog.getSceneSound();
            soundController->addSceneSound(sceneSound);

            soundTableView->addSound(sceneSound);
        }
    }

    void SoundPanelWidget::removeSelectedSound() {
        if (soundTableView->hasSceneSoundSelected()) {
            const SceneSound* sceneSound = soundTableView->getSelectedSceneSound();
            soundController->removeSceneSound(sceneSound);

            soundTableView->removeSelectedSound();
        }
    }

    void SoundPanelWidget::updateSoundGeneralProperties() {
        if (!disableSoundEvent) {
            const SceneSound* sceneSound = soundTableView->getSelectedSceneSound();

            auto initialVolume = (float)this->initialVolume->value();

            soundController->updateSceneSoundGeneralProperties(sceneSound, initialVolume);
        }
    }

    void SoundPanelWidget::updateSoundSpecificProperties() {
        if (!disableSoundEvent) {
            const SceneSound* sceneSound = soundTableView->getSelectedSceneSound();
            const Sound* sound = sceneSound->getSound();

            if (sound->getSoundType() == Sound::GLOBAL) {
                //nothing to update
            } else if (sound->getSoundType() == Sound::SPATIAL) {
                Point3<float> position((float)positionX->value(), (float)positionY->value(), (float)positionY->value());
                auto inaudibleDistance = (float)this->inaudibleDistance->value();

                soundController->updateSceneSpatialSoundProperties(sceneSound, position, inaudibleDistance);
            } else {
                throw std::invalid_argument("Unknown sound type to update specific properties: " + std::to_string(sound->getSoundType()));
            }
        }
    }

    void SoundPanelWidget::updateSoundTriggerProperties() {
        if (!disableSoundEvent) {
            const SceneSound* sceneSound = soundTableView->getSelectedSceneSound();

            QVariant playVariant = playBehavior->currentData();
            auto playBehavior = static_cast<SoundTrigger::PlayBehavior>(playVariant.toInt());

            soundController->updateSceneSoundTriggerGeneralProperties(sceneSound, playBehavior);
        }
    }

    void SoundPanelWidget::showChangeSoundTriggerDialog() {
        ChangeSoundTriggerDialog changeSoundTriggerDialog(this);
        changeSoundTriggerDialog.exec();

        if (changeSoundTriggerDialog.result() == QDialog::Accepted) {
            const SceneSound* sceneSound = soundTableView->getSelectedSceneSound();
            SoundTrigger::TriggerType triggerType = changeSoundTriggerDialog.getTriggerType();

            soundController->changeSoundTrigger(sceneSound, triggerType);
            setupSoundDataFrom(sceneSound);
        }
    }

    void SoundPanelWidget::showChangeSoundShapeDialog() {
        ChangeSoundShapeDialog changeSoundShapeDialog(this);
        changeSoundShapeDialog.exec();

        if (changeSoundShapeDialog.result() == QDialog::Accepted) {
            const SceneSound* sceneSound = soundTableView->getSelectedSceneSound();
            SoundShape::ShapeType shapeType = changeSoundShapeDialog.getShapeType();

            soundController->changeSoundShape(sceneSound, shapeType);
            setupShapeTriggerDataFrom(sceneSound);
        }
    }

    void SoundPanelWidget::soundShapeChanged(const SoundShape* soundShape) {
        if (!disableSoundEvent) {
            const SceneSound* sceneSound = soundTableView->getSelectedSceneSound();

            soundController->updateSceneSoundShape(sceneSound, soundShape);
        }
    }
}
