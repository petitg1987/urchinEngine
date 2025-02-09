#include <panel/sounds/SoundPanelWidget.h>
#include <widget/style/GroupBoxStyleHelper.h>
#include <widget/style/SpinBoxStyleHelper.h>
#include <widget/style/ButtonStyleHelper.h>
#include <widget/style/FrameStyleHelper.h>
#include <panel/sounds/dialog/NewSoundDialog.h>
#include <panel/objects/sound/dialog/ChangeSoundShapeDialog.h>
#include <panel/objects/sound/soundshape/SoundShapeWidgetRetriever.h>

namespace urchin {

    SoundPanelWidget::SoundPanelWidget() :
            soundController(nullptr),
            soundTableView(nullptr),
            addSoundButton(nullptr),
            removeSoundButton(nullptr),
            soundPropertiesGroupBox(nullptr),
            soundTriggerGroupBox(nullptr),
            specificAreaTriggerGroupBox(nullptr),
            specificLocalizableSoundGroupBox(nullptr),
            disableSoundEvent(false),
            soundType(nullptr),
            soundCategory(nullptr),
            initialVolume(nullptr),
            positionX(nullptr), positionY(nullptr), positionZ(nullptr), radius(nullptr),
            playBehavior(nullptr),
            soundTriggerType(nullptr),
            triggerShapeLayout(nullptr),
            soundShapeType(nullptr),
            changeSoundShapeTypeButton(nullptr),
            soundShapeWidget(nullptr) {
        auto* mainLayout = new QVBoxLayout(this);
        mainLayout->setAlignment(Qt::AlignTop);
        mainLayout->setContentsMargins(1, 1, 1, 1);

        soundTableView = new SoundTableView();
        mainLayout->addWidget(soundTableView);
        soundTableView->addObserver(this, SoundTableView::SOUND_SELECTION_CHANGED);
        soundTableView->setFixedHeight(320);

        auto* buttonsLayout = new QHBoxLayout();
        mainLayout->addLayout(buttonsLayout);
        buttonsLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        addSoundButton = new QPushButton("New");
        buttonsLayout->addWidget(addSoundButton);
        ButtonStyleHelper::applyNormalStyle(addSoundButton);
        connect(addSoundButton, SIGNAL(clicked()), this, SLOT(showAddSoundDialog()));

        removeSoundButton = new QPushButton("Remove");
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
        setupSpecificLocalizableSoundBox(soundPropertiesLayout);

        soundTriggerGroupBox = new QGroupBox("Sound Trigger");
        mainLayout->addWidget(soundTriggerGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(soundTriggerGroupBox);
        soundTriggerGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        soundTriggerGroupBox->hide();

        auto* soundTriggerLayout = new QVBoxLayout(soundTriggerGroupBox);
        setupSoundTriggerGeneralPropertiesBox(soundTriggerLayout);
        setupSpecificAreaTriggerBox(soundTriggerLayout);
    }

    void SoundPanelWidget::setupSoundGeneralPropertiesBox(QVBoxLayout* soundPropertiesLayout) {
        auto* generalGroupBox = new QGroupBox("General");
        soundPropertiesLayout->addWidget(generalGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(generalGroupBox);

        auto* generalPropertiesLayout = new QGridLayout(generalGroupBox);
        generalPropertiesLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        auto* soundTypeLabel= new QLabel("Sound Type:");
        generalPropertiesLayout->addWidget(soundTypeLabel, 0, 0);
        soundType = new QLabel();
        generalPropertiesLayout->addWidget(soundType, 0, 1);

        auto* soundCategoryLabel= new QLabel("Sound Category:");
        generalPropertiesLayout->addWidget(soundCategoryLabel, 1, 0);
        soundCategory = new QLabel();
        generalPropertiesLayout->addWidget(soundCategory, 1, 1);

        auto* initialVolumeLabel= new QLabel("Initial volume:");
        generalPropertiesLayout->addWidget(initialVolumeLabel, 2, 0);
        initialVolume = new QLabel();
        generalPropertiesLayout->addWidget(initialVolume, 2, 1);

    }

    void SoundPanelWidget::setupSpecificLocalizableSoundBox(QVBoxLayout* soundPropertiesLayout) {
        specificLocalizableSoundGroupBox = new QGroupBox("Localizable Sound");
        soundPropertiesLayout->addWidget(specificLocalizableSoundGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(specificLocalizableSoundGroupBox);
        specificLocalizableSoundGroupBox->hide();

        auto* localizableSoundLayout = new QGridLayout(specificLocalizableSoundGroupBox);
        localizableSoundLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        auto* positionLabel= new QLabel("Position:");
        localizableSoundLayout->addWidget(positionLabel, 0, 0);

        auto* positionLayout = new QHBoxLayout();
        localizableSoundLayout->addLayout(positionLayout, 0, 1);
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

        auto* radiusLabel= new QLabel("Radius:");
        radiusLabel->setToolTip("Inaudible Distance");
        localizableSoundLayout->addWidget(radiusLabel, 1, 0);

        radius = new QDoubleSpinBox();
        localizableSoundLayout->addWidget(radius, 1, 1);
        SpinBoxStyleHelper::applyDefaultStyleOn(radius);
        radius->setMinimum(0.0);
        connect(radius, SIGNAL(valueChanged(double)), this, SLOT(updateSoundSpecificProperties()));
    }

    void SoundPanelWidget::setupSoundTriggerGeneralPropertiesBox(QVBoxLayout* soundTriggerLayout) {
        auto* generalGroupBox = new QGroupBox("General");
        soundTriggerLayout->addWidget(generalGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(generalGroupBox);

        auto* generalLayout = new QGridLayout(generalGroupBox);
        generalLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        auto* playBehaviorLabel= new QLabel("Play Behavior:");
        generalLayout->addWidget(playBehaviorLabel, 0, 0);

        playBehavior = new QComboBox();
        generalLayout->addWidget(playBehavior, 0, 1, 1, 3);
        playBehavior->addItem(PLAY_ONCE_LABEL, QVariant((int)PlayBehavior::PLAY_ONCE));
        playBehavior->addItem(PLAY_LOOP_LABEL, QVariant((int)PlayBehavior::PLAY_LOOP));
        connect(playBehavior, SIGNAL(currentIndexChanged(int)), this, SLOT(updateSoundTriggerProperties()));

        auto* soundTriggerTypeLabel= new QLabel("Trigger:");
        generalLayout->addWidget(soundTriggerTypeLabel, 3, 0);

        soundTriggerType = new QLabel();
        generalLayout->addWidget(soundTriggerType, 3, 1);
    }

    void SoundPanelWidget::setupSpecificAreaTriggerBox(QVBoxLayout* soundTriggerLayout) {
        specificAreaTriggerGroupBox = new QGroupBox("Area Trigger");
        soundTriggerLayout->addWidget(specificAreaTriggerGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(specificAreaTriggerGroupBox);
        specificAreaTriggerGroupBox->hide();

        triggerShapeLayout = new QVBoxLayout(specificAreaTriggerGroupBox);

        auto* shapeTypeLayout = new QHBoxLayout();
        shapeTypeLayout->setAlignment(Qt::AlignLeft);
        shapeTypeLayout->setSpacing(15);
        triggerShapeLayout->addLayout(shapeTypeLayout);

        auto* soundShapeTypeLabel= new QLabel("Shape:");
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

    void SoundPanelWidget::load(SoundController& soundController) {
        this->soundController = &soundController;

        std::list<const SoundEntity*> soundsEntities = this->soundController->getSoundEntities();
        for (auto& soundsEntity : soundsEntities) {
            soundTableView->addSound(*soundsEntity);
        }
    }

    void SoundPanelWidget::unload() {
        soundTableView->removeAllSounds();

        soundController = nullptr;
    }

    void SoundPanelWidget::notify(Observable* observable, int notificationType) {
        if (const auto* soundTableView = dynamic_cast<SoundTableView*>(observable)) {
            if (notificationType == SoundTableView::SOUND_SELECTION_CHANGED) {
                if (soundTableView->hasSoundEntitySelected()) {
                    const SoundEntity& soundEntity = *soundTableView->getSelectedSoundEntity();
                    setupSoundDataFrom(soundEntity);

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

    void SoundPanelWidget::setupSoundDataFrom(const SoundEntity& soundEntity) {
        disableSoundEvent = true;

        //sound
        const Sound& sound = soundEntity.getSoundComponent()->getSound();

        if (sound.getSoundType() == Sound::SoundType::GLOBAL) {
            setupGlobalSoundDataFrom();
        } else if (sound.getSoundType() == Sound::SoundType::LOCALIZABLE) {
            setupLocalizableSoundDataFrom(static_cast<const LocalizableSound&>(sound));
        } else {
            throw std::invalid_argument("Impossible to setup specific sound data for sound of type: " + std::to_string((int)sound.getSoundType()));
        }

        if (sound.getSoundCategory() == Sound::SoundCategory::MUSIC) {
            soundCategory->setText(NewSoundDialog::MUSIC_SOUND_LABEL);
        } else if (sound.getSoundCategory() == Sound::SoundCategory::EFFECTS) {
            soundCategory->setText(NewSoundDialog::EFFECTS_SOUND_LABEL);
        } else {
            throw std::invalid_argument("Impossible to setup specific sound data for sound of category: " + std::to_string((int)sound.getSoundCategory()));
        }

        initialVolume->setText(TypeConverter::toString(sound.getInitialVolume()).c_str());

        //sound trigger
        const SoundTrigger& soundTrigger = soundEntity.getSoundComponent()->getSoundTrigger();
        setupPlayBehaviorDataFrom(soundTrigger);

        if (soundTrigger.getTriggerType() == SoundTrigger::TriggerType::AUTO_TRIGGER) {
            setupAutoTriggerDataFrom();
        } else if (soundTrigger.getTriggerType() == SoundTrigger::TriggerType::MANUAL_TRIGGER) {
            setupManualTriggerDataFrom();
        } else if (soundTrigger.getTriggerType() == SoundTrigger::TriggerType::AREA_TRIGGER) {
            setupShapeTriggerDataFrom(soundEntity);
        } else {
            throw std::invalid_argument("Impossible to setup specific sound trigger data for sound trigger of type: " + std::to_string(soundTrigger.getTriggerType()));
        }

        disableSoundEvent = false;
    }

    void SoundPanelWidget::setupGlobalSoundDataFrom() const {
        specificLocalizableSoundGroupBox->hide();

        soundType->setText(NewSoundDialog::GLOBAL_SOUND_LABEL);
    }

    void SoundPanelWidget::setupLocalizableSoundDataFrom(const LocalizableSound& localizableSound) const {
        specificLocalizableSoundGroupBox->show();

        soundType->setText(NewSoundDialog::LOCALIZABLE_SOUND_LABEL);

        this->positionX->setValue(localizableSound.getPosition().X);
        this->positionY->setValue(localizableSound.getPosition().Y);
        this->positionZ->setValue(localizableSound.getPosition().Z);

        this->radius->setValue(localizableSound.getRadius());
    }

    void SoundPanelWidget::setupPlayBehaviorDataFrom(const SoundTrigger& soundTrigger) const {
        int playBehaviorIndex = playBehavior->findData((int)soundTrigger.getPlayBehavior());
        if (playBehaviorIndex != -1) {
            playBehavior->setCurrentIndex(playBehaviorIndex);
        }
    }

    void SoundPanelWidget::setupAutoTriggerDataFrom() const {
        specificAreaTriggerGroupBox->hide();
        soundTriggerType->setText(NewSoundDialog::AUTO_TRIGGER_LABEL);
    }

    void SoundPanelWidget::setupManualTriggerDataFrom() const {
        specificAreaTriggerGroupBox->hide();
        soundTriggerType->setText(NewSoundDialog::MANUAL_TRIGGER_LABEL);
    }

    void SoundPanelWidget::setupShapeTriggerDataFrom(const SoundEntity& soundEntity) {
        specificAreaTriggerGroupBox->show();
        soundTriggerType->setText(NewSoundDialog::AREA_TRIGGER_LABEL);

        const auto& soundShape = soundEntity.getSoundComponent()->getAreaTrigger().getSoundShape();
        SoundShapeWidget& soundShapeWidget = retrieveSoundShapeWidget(soundShape);
        soundShapeWidget.setupShapePropertiesFrom(soundShape);

        soundShapeType->setText(QString::fromStdString(soundShapeWidget.getSoundShapeName()));
    }

    SoundShapeWidget& SoundPanelWidget::retrieveSoundShapeWidget(const SoundShape& shape) {
        soundShapeWidget = SoundShapeWidgetRetriever().retrieveShapeWidget(shape.getShapeType());

        QString soundShapeWidgetName = QString::fromStdString("soundShapeWidget");

        for (int i = 0; i < triggerShapeLayout->count(); ++i) {
            QWidget* oldSoundShapeWidget = triggerShapeLayout->itemAt(i)->widget();
            if(oldSoundShapeWidget && oldSoundShapeWidget->objectName() == soundShapeWidgetName) {
                triggerShapeLayout->removeWidget(oldSoundShapeWidget);
                oldSoundShapeWidget->deleteLater();
            }
        }

        soundShapeWidget->setObjectName(soundShapeWidgetName);
        triggerShapeLayout->addWidget(soundShapeWidget);
        soundShapeWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        soundShapeWidget->show();
        connect(soundShapeWidget, SIGNAL(soundShapeChange(SoundShape*)), this, SLOT(soundShapeChanged(SoundShape*)));

        return *soundShapeWidget;
    }

    void SoundPanelWidget::showAddSoundDialog() {
        NewSoundDialog newSoundEntityDialog(this, soundController);
        newSoundEntityDialog.exec();

        if (newSoundEntityDialog.result() == QDialog::Accepted) {
            std::unique_ptr<SoundEntity> soundEntity = newSoundEntityDialog.moveSoundEntity();
            const SoundEntity* soundEntityPtr = soundEntity.get();
            soundController->addSoundEntity(std::move(soundEntity));
            soundController->moveSoundInFrontOfCamera(*soundEntityPtr, false);

            int row = soundTableView->addSound(*soundEntityPtr);
            soundTableView->selectRow(row);
        }
    }

    void SoundPanelWidget::removeSelectedSound() const {
        if (soundTableView->hasSoundEntitySelected()) {
            const SoundEntity& soundEntity = *soundTableView->getSelectedSoundEntity();
            soundController->removeSoundEntity(soundEntity);

            soundTableView->removeSelectedSound();
        }
    }

    void SoundPanelWidget::updateSoundSpecificProperties() const {
        if (!disableSoundEvent) {
            const SoundEntity& soundEntity = *soundTableView->getSelectedSoundEntity();
            const Sound& sound = soundEntity.getSoundComponent()->getSound();

            if (sound.getSoundType() == Sound::SoundType::GLOBAL) {
                //nothing to update
            } else if (sound.getSoundType() == Sound::SoundType::LOCALIZABLE) {
                Point3 position((float)positionX->value(), (float)positionY->value(), (float)positionZ->value());
                auto radius = (float)this->radius->value();

                soundController->updateLocalizableSoundProperties(soundEntity, position, radius);
            } else {
                throw std::invalid_argument("Unknown sound type to update specific properties: " + std::to_string((int)sound.getSoundType()));
            }
        }
    }

    void SoundPanelWidget::updateSoundTriggerProperties() const {
        if (!disableSoundEvent) {
            const SoundEntity& soundEntity = *soundTableView->getSelectedSoundEntity();

            QVariant playVariant = playBehavior->currentData();
            auto playBehavior = static_cast<PlayBehavior>(playVariant.toInt());

            soundController->updateSoundTriggerGeneralProperties(soundEntity, playBehavior);
        }
    }

    void SoundPanelWidget::showChangeSoundShapeDialog() {
        ChangeSoundShapeDialog changeSoundShapeDialog(this);
        changeSoundShapeDialog.exec();

        if (changeSoundShapeDialog.result() == QDialog::Accepted) {
            const SoundEntity& soundEntity = *soundTableView->getSelectedSoundEntity();
            SoundShape::ShapeType shapeType = changeSoundShapeDialog.getShapeType();

            soundController->changeSoundShape(soundEntity, shapeType);
            setupShapeTriggerDataFrom(soundEntity);
        }
    }

    void SoundPanelWidget::soundShapeChanged(SoundShape* soundShape) const {
        if (!disableSoundEvent) {
            const SoundEntity& soundEntity = *soundTableView->getSelectedSoundEntity();

            soundController->updateSoundShape(soundEntity, std::unique_ptr<SoundShape>(soundShape));
        }
    }
}
