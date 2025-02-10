#include <panel/objects/sound/SoundWidget.h>
#include <widget/style/GroupBoxStyleHelper.h>
#include <widget/style/SpinBoxStyleHelper.h>
#include <widget/style/ButtonStyleHelper.h>
#include <widget/style/FrameStyleHelper.h>

#include <panel/objects/sound/dialog/ChangeSoundShapeTypeDialog.h>
#include <panel/objects/sound/dialog/ChangeSoundTriggerTypeDialog.h>
#include <panel/objects/sound/soundshape/SoundShapeWidgetRetriever.h>
#include <panel/objects/sound/soundshape/support/DefaultSoundShapeCreator.h>
#include <panel/objects/dialog/ChangeSoundDialog.h>

namespace urchin {

    SoundWidget::SoundWidget() :
            objectEntity(nullptr),
            objectController(nullptr),
            specificAreaTriggerGroupBox(nullptr),
            specificLocalizableSoundGroupBox(nullptr) {
        setContentsMargins(0, 0, 0, 0);

        auto* mainLayout = new QVBoxLayout(this);
        mainLayout->setAlignment(Qt::AlignTop);
        mainLayout->setContentsMargins(0, 0, 0, 0);

        auto* soundPropertiesGroupBox = new QGroupBox("Sound Properties");
        mainLayout->addWidget(soundPropertiesGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(soundPropertiesGroupBox);
        soundPropertiesGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

        auto* soundPropertiesLayout = new QVBoxLayout(soundPropertiesGroupBox);
        setupSoundGeneralPropertiesBox(soundPropertiesLayout);
        setupSpecificLocalizableSoundBox(soundPropertiesLayout);

        auto* soundTriggerGroupBox = new QGroupBox("Sound Trigger");
        mainLayout->addWidget(soundTriggerGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(soundTriggerGroupBox);
        soundTriggerGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

        auto* soundTriggerLayout = new QVBoxLayout(soundTriggerGroupBox);
        setupSoundTriggerGeneralPropertiesBox(soundTriggerLayout);
        setupSpecificAreaTriggerBox(soundTriggerLayout);
    }

    void SoundWidget::load(const ObjectEntity& objectEntity, ObjectController& objectController) {
        this->objectEntity = &objectEntity;
        this->objectController = &objectController;

        if (objectEntity.getSoundComponent()) {
            setupSoundDataFrom();
        }
    }

    void SoundWidget::setupSoundGeneralPropertiesBox(QVBoxLayout* soundPropertiesLayout) {
        auto* generalGroupBox = new QGroupBox("General");
        soundPropertiesLayout->addWidget(generalGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(generalGroupBox);

        auto* generalPropertiesLayout = new QGridLayout(generalGroupBox);
        generalPropertiesLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        auto* soundFileLabel= new QLabel("Sound File:");
        generalPropertiesLayout->addWidget(soundFileLabel, 0, 0);
        soundFile = new QLabel();
        generalPropertiesLayout->addWidget(soundFile, 0, 1);

        auto* soundCategoryLabel= new QLabel("Sound Category:");
        generalPropertiesLayout->addWidget(soundCategoryLabel, 1, 0);

        soundCategory = new QComboBox();
        generalPropertiesLayout->addWidget(soundCategory, 1, 1);
        soundCategory->addItem(MUSIC_SOUND_LABEL, QVariant((int)Sound::SoundCategory::MUSIC));
        soundCategory->addItem(EFFECTS_SOUND_LABEL, QVariant((int)Sound::SoundCategory::EFFECTS));
        connect(soundCategory, SIGNAL(currentIndexChanged(int)), this, SLOT(updateSoundComponent()));

        auto* initialVolumeLabel= new QLabel("Initial volume:");
        generalPropertiesLayout->addWidget(initialVolumeLabel, 2, 0);
        initialVolume = new QDoubleSpinBox();
        generalPropertiesLayout->addWidget(initialVolume);
        SpinBoxStyleHelper::applyDefaultStyleOn(initialVolume);
        initialVolume->setMinimum(0.0);
        initialVolume->setMaximum(1.0);
        connect(initialVolume, SIGNAL(valueChanged(double)), this, SLOT(updateSoundComponent()));
    }

    void SoundWidget::setupSpecificLocalizableSoundBox(QVBoxLayout* soundPropertiesLayout) {
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
        connect(positionX, SIGNAL(valueChanged(double)), this, SLOT(updateSoundComponent()));
        positionY = new QDoubleSpinBox();
        positionLayout->addWidget(positionY);
        SpinBoxStyleHelper::applyDefaultStyleOn(positionY);
        connect(positionY, SIGNAL(valueChanged(double)), this, SLOT(updateSoundComponent()));
        positionZ = new QDoubleSpinBox();
        positionLayout->addWidget(positionZ);
        SpinBoxStyleHelper::applyDefaultStyleOn(positionZ);
        connect(positionZ, SIGNAL(valueChanged(double)), this, SLOT(updateSoundComponent()));

        auto* radiusLabel= new QLabel("Radius:");
        radiusLabel->setToolTip("Inaudible Distance");
        localizableSoundLayout->addWidget(radiusLabel, 1, 0);

        radius = new QDoubleSpinBox();
        localizableSoundLayout->addWidget(radius, 1, 1);
        SpinBoxStyleHelper::applyDefaultStyleOn(radius);
        radius->setMinimum(0.0);
        connect(radius, SIGNAL(valueChanged(double)), this, SLOT(updateSoundComponent()));
    }

    void SoundWidget::setupSoundTriggerGeneralPropertiesBox(QVBoxLayout* soundTriggerLayout) {
        auto* generalGroupBox = new QGroupBox("General");
        soundTriggerLayout->addWidget(generalGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(generalGroupBox);

        auto* generalLayout = new QGridLayout(generalGroupBox);
        generalLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        auto* playBehaviorLabel= new QLabel("Play Behavior:");
        generalLayout->addWidget(playBehaviorLabel, 0, 0);

        playBehavior = new QComboBox();
        generalLayout->addWidget(playBehavior, 0, 1);
        playBehavior->addItem(PLAY_ONCE_LABEL, QVariant((int)PlayBehavior::PLAY_ONCE));
        playBehavior->addItem(PLAY_LOOP_LABEL, QVariant((int)PlayBehavior::PLAY_LOOP));
        connect(playBehavior, SIGNAL(currentIndexChanged(int)), this, SLOT(updateSoundComponent()));

        auto* triggerTypeLayout = new QHBoxLayout();
        triggerTypeLayout->setAlignment(Qt::AlignLeft);
        triggerTypeLayout->setSpacing(15);
        generalLayout->addLayout(triggerTypeLayout, 1, 0, 1, 2);

        auto* soundTriggerTypeLabel= new QLabel("Trigger Type:");
        triggerTypeLayout->addWidget(soundTriggerTypeLabel);

        soundTriggerType = new QLabel();
        triggerTypeLayout->addWidget(soundTriggerType);

        changeSoundTriggerTypeButton = new QPushButton("Change");
        triggerTypeLayout->addWidget(changeSoundTriggerTypeButton);
        ButtonStyleHelper::applyNormalStyle(changeSoundTriggerTypeButton);
        connect(changeSoundTriggerTypeButton, SIGNAL(clicked()), this, SLOT(showChangeSoundTriggerTypeDialog()));
    }

    void SoundWidget::setupSpecificAreaTriggerBox(QVBoxLayout* soundTriggerLayout) {
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
        connect(changeSoundShapeTypeButton, SIGNAL(clicked()), this, SLOT(showChangeSoundShapeTypeDialog()));

        auto* frameLine = new QFrame();
        triggerShapeLayout->addWidget(frameLine);
        FrameStyleHelper::applyLineStyle(frameLine);

        soundShapeWidget = nullptr;
    }

    void SoundWidget::setupSoundDataFrom() {
        disableSoundEvent = true;

        //sound
        const Sound& sound = objectEntity->getSoundComponent()->getSound();

        std::string soundPathFilename = sound.getFilename();
        std::string soundFilename = FileUtil::getFileName(soundPathFilename);
        soundFile->setText(soundFilename.c_str());
        soundFile->setToolTip(soundPathFilename.c_str());

        int soundCategoryIndex = soundCategory->findData((int)sound.getSoundCategory());
        if (soundCategoryIndex != -1) {
            soundCategory->setCurrentIndex(soundCategoryIndex);
        }

        initialVolume->setValue(sound.getInitialVolume());

        if (sound.getSoundType() == Sound::SoundType::GLOBAL) {
            setupGlobalSoundDataFrom();
        } else if (sound.getSoundType() == Sound::SoundType::LOCALIZABLE) {
            setupLocalizableSoundDataFrom(static_cast<const LocalizableSound&>(sound));
        } else {
            throw std::invalid_argument("Impossible to setup specific sound data for sound of type: " + std::to_string((int)sound.getSoundType()));
        }

        //sound trigger
        const SoundTrigger& soundTrigger = objectEntity->getSoundComponent()->getSoundTrigger();
        setupPlayBehaviorDataFrom(soundTrigger);

        if (soundTrigger.getTriggerType() == SoundTrigger::TriggerType::AUTO_TRIGGER) {
            setupAutoTriggerDataFrom();
        } else if (soundTrigger.getTriggerType() == SoundTrigger::TriggerType::MANUAL_TRIGGER) {
            setupManualTriggerDataFrom();
        } else if (soundTrigger.getTriggerType() == SoundTrigger::TriggerType::AREA_TRIGGER) {
            setupShapeTriggerDataFrom();
        } else {
            throw std::invalid_argument("Impossible to setup specific sound trigger data for sound trigger of type: " + std::to_string(soundTrigger.getTriggerType()));
        }

        disableSoundEvent = false;
    }

    void SoundWidget::setupGlobalSoundDataFrom() const {
        specificLocalizableSoundGroupBox->hide();
    }

    void SoundWidget::setupLocalizableSoundDataFrom(const LocalizableSound& localizableSound) const {
        specificLocalizableSoundGroupBox->show();

        this->positionX->setValue(localizableSound.getPosition().X);
        this->positionY->setValue(localizableSound.getPosition().Y);
        this->positionZ->setValue(localizableSound.getPosition().Z);

        this->radius->setValue(localizableSound.getRadius());
    }

    void SoundWidget::setupPlayBehaviorDataFrom(const SoundTrigger& soundTrigger) const {
        int playBehaviorIndex = playBehavior->findData((int)soundTrigger.getPlayBehavior());
        if (playBehaviorIndex != -1) {
            playBehavior->setCurrentIndex(playBehaviorIndex);
        }
    }

    void SoundWidget::setupAutoTriggerDataFrom() const {
        specificAreaTriggerGroupBox->hide();
        soundTriggerType->setText(ChangeSoundTriggerTypeDialog::AUTO_TRIGGER_LABEL);
    }

    void SoundWidget::setupManualTriggerDataFrom() const {
        specificAreaTriggerGroupBox->hide();
        soundTriggerType->setText(ChangeSoundTriggerTypeDialog::MANUAL_TRIGGER_LABEL);
    }

    void SoundWidget::setupShapeTriggerDataFrom() {
        specificAreaTriggerGroupBox->show();
        soundTriggerType->setText(ChangeSoundTriggerTypeDialog::AREA_TRIGGER_LABEL);

        const auto& soundShape = objectEntity->getSoundComponent()->getAreaTrigger().getSoundShape();
        SoundShapeWidget& soundShapeWidget = retrieveSoundShapeWidget(soundShape);
        soundShapeWidget.setupShapePropertiesFrom(soundShape);

        soundShapeType->setText(QString::fromStdString(soundShapeWidget.getSoundShapeName()));
    }

    SoundShapeWidget& SoundWidget::retrieveSoundShapeWidget(const SoundShape& shape) {
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

    void SoundWidget::updateSoundComponent() const {
        if (!disableSoundEvent) {
            //sound
            const Sound& sound = objectEntity->getSoundComponent()->getSound();

            std::string filename = soundFile->toolTip().toStdString();

            QVariant soundCategoryVariant = soundCategory->currentData();
            auto soundCategoryValue = static_cast<Sound::SoundCategory>(soundCategoryVariant.toInt());

            float initialVolumeValue = (float)initialVolume->value();

            std::shared_ptr<Sound> newSound;
            if (sound.getSoundType() == Sound::SoundType::LOCALIZABLE) {
                Point3 position((float)positionX->value(), (float)positionY->value(), (float)positionZ->value());
                auto radius = (float)this->radius->value();
                newSound = std::make_shared<LocalizableSound>(filename, soundCategoryValue, initialVolumeValue, position, radius);
            } else if (sound.getSoundType() == Sound::SoundType::GLOBAL) {
                newSound = std::make_shared<GlobalSound>(filename, soundCategoryValue, initialVolumeValue);
            } else {
                throw std::invalid_argument("Unknown sound type to update sound component: " + std::to_string((int)sound.getSoundType()));
            }

            //sound trigger
            const SoundTrigger& soundTrigger = objectEntity->getSoundComponent()->getSoundTrigger();

            QVariant playBehaviorVariant = playBehavior->currentData();
            auto playBehavior = static_cast<PlayBehavior>(playBehaviorVariant.toInt());

            std::shared_ptr<SoundTrigger> newSoundTrigger;
            if (soundTrigger.getTriggerType() == SoundTrigger::AUTO_TRIGGER) {
                newSoundTrigger = std::make_shared<AutoTrigger>(playBehavior, newSound);
            } else if (soundTrigger.getTriggerType() == SoundTrigger::MANUAL_TRIGGER) {
                newSoundTrigger = std::make_shared<ManualTrigger>(playBehavior);
            } else if (soundTrigger.getTriggerType() == SoundTrigger::AREA_TRIGGER) {
                AreaTrigger& areaTrigger = objectEntity->getSoundComponent()->getAreaTrigger();
                std::unique_ptr areaSoundShape = areaTrigger.getSoundShape().clone();
                newSoundTrigger = std::make_shared<AreaTrigger>(playBehavior, std::move(areaSoundShape));
            } else {
                throw std::invalid_argument("Unknown the trigger type to create a new sound trigger: " + std::to_string(soundTrigger.getTriggerType()));
            }

            objectController->updateSoundComponent(*objectEntity, newSound, newSoundTrigger);
        }
    }

    void SoundWidget::showChangeSoundTriggerTypeDialog() {
        ChangeSoundTriggerTypeDialog changeSoundTriggerTypeDialog(this);
        changeSoundTriggerTypeDialog.exec();

        if (changeSoundTriggerTypeDialog.result() == QDialog::Accepted) {
            SoundTrigger::TriggerType triggerType = changeSoundTriggerTypeDialog.getSoundTriggerType();
            if (triggerType != objectEntity->getSoundComponent()->getSoundTrigger().getTriggerType()) {
                std::shared_ptr clonedSound = objectEntity->getSoundComponent()->getSound().clone();

                std::shared_ptr<SoundTrigger> newSoundTrigger;
                if (triggerType == SoundTrigger::AUTO_TRIGGER) {
                    newSoundTrigger = std::make_shared<AutoTrigger>(PlayBehavior::PLAY_ONCE, clonedSound);
                } else if (triggerType == SoundTrigger::MANUAL_TRIGGER) {
                    newSoundTrigger = std::make_shared<ManualTrigger>(PlayBehavior::PLAY_ONCE);
                } else if (triggerType == SoundTrigger::AREA_TRIGGER) {
                    auto newDefaultShape = DefaultSoundShapeCreator(*clonedSound).createDefaultSoundShape(SoundShape::SPHERE_SHAPE);
                    newSoundTrigger = std::make_shared<AreaTrigger>(PlayBehavior::PLAY_ONCE, std::move(newDefaultShape));
                } else {
                    throw std::invalid_argument("Unknown the trigger type to create a new sound trigger: " + std::to_string(triggerType));
                }

                objectController->updateSoundComponent(*objectEntity, std::move(clonedSound), std::move(newSoundTrigger));
                setupSoundDataFrom();
            }
        }
    }

    void SoundWidget::showChangeSoundShapeTypeDialog() {
        ChangeSoundShapeTypeDialog changeSoundShapeTypeDialog(this);
        changeSoundShapeTypeDialog.exec();

        if (changeSoundShapeTypeDialog.result() == QDialog::Accepted) {
            SoundShape::ShapeType shapeType = changeSoundShapeTypeDialog.getShapeType();

            objectController->changeSoundShape(*objectEntity, shapeType);
            setupShapeTriggerDataFrom();
        }
    }

    void SoundWidget::soundShapeChanged(SoundShape* soundShape) const {
        if (!disableSoundEvent) {
            objectController->updateSoundShape(*objectEntity, std::unique_ptr<SoundShape>(soundShape));
        }
    }

}
