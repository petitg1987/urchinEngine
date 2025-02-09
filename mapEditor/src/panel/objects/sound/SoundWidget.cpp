#include <panel/objects/sound/SoundWidget.h>
#include <widget/style/GroupBoxStyleHelper.h>
#include <widget/style/SpinBoxStyleHelper.h>
#include <widget/style/ButtonStyleHelper.h>
#include <widget/style/FrameStyleHelper.h>

#include <panel/objects/sound/dialog/ChangeSoundShapeDialog.h>
#include <panel/objects/sound/soundshape/SoundShapeWidgetRetriever.h>
#include <panel/objects/dialog/ChangeSoundDialog.h>

namespace urchin {

    SoundWidget::SoundWidget() :
            objectEntity(nullptr),
            objectController(nullptr),
            soundPropertiesGroupBox(nullptr),
            soundTriggerGroupBox(nullptr),
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

    void SoundWidget::setupSoundTriggerGeneralPropertiesBox(QVBoxLayout* soundTriggerLayout) {
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
        connect(changeSoundShapeTypeButton, SIGNAL(clicked()), this, SLOT(showChangeSoundShapeDialog()));

        auto* frameLine = new QFrame();
        triggerShapeLayout->addWidget(frameLine);
        FrameStyleHelper::applyLineStyle(frameLine);

        soundShapeWidget = nullptr;
    }

    void SoundWidget::setupSoundDataFrom() {
        disableSoundEvent = true;

        //sound
        const Sound& sound = objectEntity->getSoundComponent()->getSound();

        if (sound.getSoundType() == Sound::SoundType::GLOBAL) {
            setupGlobalSoundDataFrom();
        } else if (sound.getSoundType() == Sound::SoundType::LOCALIZABLE) {
            setupLocalizableSoundDataFrom(static_cast<const LocalizableSound&>(sound));
        } else {
            throw std::invalid_argument("Impossible to setup specific sound data for sound of type: " + std::to_string((int)sound.getSoundType()));
        }

        if (sound.getSoundCategory() == Sound::SoundCategory::MUSIC) {
            soundCategory->setText(MUSIC_SOUND_LABEL);
        } else if (sound.getSoundCategory() == Sound::SoundCategory::EFFECTS) {
            soundCategory->setText(EFFECTS_SOUND_LABEL);
        } else {
            throw std::invalid_argument("Impossible to setup specific sound data for sound of category: " + std::to_string((int)sound.getSoundCategory()));
        }

        initialVolume->setText(TypeConverter::toString(sound.getInitialVolume()).c_str());

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

        soundType->setText(ChangeSoundDialog::GLOBAL_SOUND_LABEL);
    }

    void SoundWidget::setupLocalizableSoundDataFrom(const LocalizableSound& localizableSound) const {
        specificLocalizableSoundGroupBox->show();

        soundType->setText(ChangeSoundDialog::LOCALIZABLE_SOUND_LABEL);

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
        soundTriggerType->setText(AUTO_TRIGGER_LABEL);
    }

    void SoundWidget::setupManualTriggerDataFrom() const {
        specificAreaTriggerGroupBox->hide();
        soundTriggerType->setText(MANUAL_TRIGGER_LABEL);
    }

    void SoundWidget::setupShapeTriggerDataFrom() {
        specificAreaTriggerGroupBox->show();
        soundTriggerType->setText(AREA_TRIGGER_LABEL);

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

    void SoundWidget::updateSoundSpecificProperties() const {
        if (!disableSoundEvent) {
            const Sound& sound = objectEntity->getSoundComponent()->getSound();

            if (sound.getSoundType() == Sound::SoundType::GLOBAL) {
                //nothing to update
            } else if (sound.getSoundType() == Sound::SoundType::LOCALIZABLE) {
                Point3 position((float)positionX->value(), (float)positionY->value(), (float)positionZ->value());
                auto radius = (float)this->radius->value();

                objectController->updateLocalizableSoundProperties(*objectEntity, position, radius);
            } else {
                throw std::invalid_argument("Unknown sound type to update specific properties: " + std::to_string((int)sound.getSoundType()));
            }
        }
    }

    void SoundWidget::updateSoundTriggerProperties() const {
        if (!disableSoundEvent) {
            QVariant playVariant = playBehavior->currentData();
            auto playBehavior = static_cast<PlayBehavior>(playVariant.toInt());

            objectController->updateSoundTriggerGeneralProperties(*objectEntity, playBehavior);
        }
    }

    void SoundWidget::showChangeSoundShapeDialog() {
        ChangeSoundShapeDialog changeSoundShapeDialog(this);
        changeSoundShapeDialog.exec();

        if (changeSoundShapeDialog.result() == QDialog::Accepted) {
            SoundShape::ShapeType shapeType = changeSoundShapeDialog.getShapeType();

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
