#include <stdexcept>
#include <QMessageBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFileDialog>

#include <UrchinCommon.h>
#include <panel/sounds/dialog/NewSoundDialog.h>
#include <panel/objects/sound/soundshape/support/DefaultSoundShapeCreator.h>
#include <widget/style/LabelStyleHelper.h>
#include <widget/style/ButtonStyleHelper.h>
#include <widget/style/SpinBoxStyleHelper.h>

#include "trigger/AutoTrigger.h"

namespace urchin {

    QString NewSoundDialog::preferredSoundPath = QString();

    NewSoundDialog::NewSoundDialog(QWidget* parent, const SoundController* soundController) :
            QDialog(parent),
            soundController(soundController),
            soundNameLabel(nullptr),
            soundNameText(nullptr),
            soundFilenameLabel(nullptr),
            soundFilenameText(nullptr),
            soundTypeComboBox(nullptr),
            soundCategoryComboBox(nullptr),
            initialVolumeSpinBox(nullptr),
            soundTriggerTypeComboBox(nullptr),
            soundEntity(nullptr) {
        this->setWindowTitle("New Sound");
        this->resize(530, 240);
        this->setFixedSize(this->width(), this->height());

        auto* mainLayout = new QGridLayout(this);
        mainLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        setupNameFields(mainLayout);
        setupSoundFilenameFields(mainLayout);
        setupSoundTypeFields(mainLayout);
        setupSoundCategoryFields(mainLayout);
        setupSoundInitialVolume(mainLayout);
        setupSoundTriggerTypeFields(mainLayout);

        auto* buttonBox = new QDialogButtonBox();
        mainLayout->addWidget(buttonBox, 6, 0, 1, 3, Qt::AlignRight);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

        connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    }

    void NewSoundDialog::setupNameFields(QGridLayout* mainLayout) {
        soundNameLabel = new QLabel("Sound Name:");
        mainLayout->addWidget(soundNameLabel, 0, 0);

        soundNameText = new QLineEdit();
        mainLayout->addWidget(soundNameText, 0, 1);
        soundNameText->setFixedWidth(360);
    }

    void NewSoundDialog::setupSoundFilenameFields(QGridLayout* mainLayout) {
        soundFilenameLabel = new QLabel("Sound File:");
        mainLayout->addWidget(soundFilenameLabel, 1, 0);

        soundFilenameText = new QLineEdit();
        mainLayout->addWidget(soundFilenameText, 1, 1);
        soundFilenameText->setReadOnly(true);
        soundFilenameText->setFixedWidth(360);

        auto* selectMeshFileButton = new QPushButton("...");
        mainLayout->addWidget(selectMeshFileButton, 1, 2);
        ButtonStyleHelper::applyNormalStyle(selectMeshFileButton);
        selectMeshFileButton->setFixedWidth(22);
        connect(selectMeshFileButton, SIGNAL(clicked()), this, SLOT(showSoundFilenameDialog()));
    }

    void NewSoundDialog::setupSoundTypeFields(QGridLayout* mainLayout) {
        auto* soundTypeLabel = new QLabel("Sound Type:");
        mainLayout->addWidget(soundTypeLabel, 2, 0);

        soundTypeComboBox = new QComboBox();
        mainLayout->addWidget(soundTypeComboBox, 2, 1);
        soundTypeComboBox->setFixedWidth(150);
        soundTypeComboBox->addItem(GLOBAL_SOUND_LABEL, QVariant((int)Sound::SoundType::GLOBAL));
        soundTypeComboBox->addItem(LOCALIZABLE_SOUND_LABEL, QVariant((int)Sound::SoundType::LOCALIZABLE));
    }

    void NewSoundDialog::setupSoundCategoryFields(QGridLayout* mainLayout) {
        auto* soundCategoryLabel = new QLabel("Sound Category:");
        mainLayout->addWidget(soundCategoryLabel, 3, 0);

        soundCategoryComboBox = new QComboBox();
        mainLayout->addWidget(soundCategoryComboBox, 3, 1);
        soundCategoryComboBox->setFixedWidth(150);
        soundCategoryComboBox->addItem(MUSIC_SOUND_LABEL, QVariant((int)Sound::SoundCategory::MUSIC));
        soundCategoryComboBox->addItem(EFFECTS_SOUND_LABEL, QVariant((int)Sound::SoundCategory::EFFECTS));
    }

    void NewSoundDialog::setupSoundInitialVolume(QGridLayout* mainLayout) {
        auto* soundInitialVolumeLabel= new QLabel("Initial Volume:");
        mainLayout->addWidget(soundInitialVolumeLabel, 4, 0);

        initialVolumeSpinBox = new QDoubleSpinBox();
        mainLayout->addWidget(initialVolumeSpinBox, 4, 1);
        SpinBoxStyleHelper::applyDefaultStyleOn(initialVolumeSpinBox);
        initialVolumeSpinBox->setMinimum(0.0);
        initialVolumeSpinBox->setMaximum(1.0);
        initialVolumeSpinBox->setValue(1.0);
    }

    void NewSoundDialog::setupSoundTriggerTypeFields(QGridLayout* mainLayout) {
        auto* soundTriggerTypeLabel = new QLabel("Trigger Type:");
        mainLayout->addWidget(soundTriggerTypeLabel, 5, 0);

        soundTriggerTypeComboBox = new QComboBox();
        mainLayout->addWidget(soundTriggerTypeComboBox, 5, 1);
        soundTriggerTypeComboBox->setFixedWidth(150);
        soundTriggerTypeComboBox->addItem(AUTO_TRIGGER_LABEL, QVariant(SoundTrigger::TriggerType::AUTO_TRIGGER));
        soundTriggerTypeComboBox->addItem(MANUAL_TRIGGER_LABEL, QVariant(SoundTrigger::TriggerType::MANUAL_TRIGGER));
        soundTriggerTypeComboBox->addItem(AREA_TRIGGER_LABEL, QVariant(SoundTrigger::TriggerType::AREA_TRIGGER));
    }

    void NewSoundDialog::updateSoundName() {
        QString soundName = soundNameText->text();
        if (!soundName.isEmpty()) {
            this->soundName = soundName.toUtf8().constData();
        }
    }

    int NewSoundDialog::buildSoundEntity(int result) {
        try {
            soundEntity = std::make_unique<SoundEntity>();

            soundEntity->setName(soundName);

            QVariant soundTypeVariant = soundTypeComboBox->currentData();
            auto soundType = static_cast<Sound::SoundType>(soundTypeVariant.toInt());

            QVariant soundCategoryVariant = soundCategoryComboBox->currentData();
            auto soundCategory = static_cast<Sound::SoundCategory>(soundCategoryVariant.toInt());

            auto initialVolume = (float)initialVolumeSpinBox->value();

            std::shared_ptr<Sound> sound;
            if (soundType == Sound::SoundType::GLOBAL) {
                sound = std::make_shared<GlobalSound>(soundFilename, soundCategory, initialVolume);
            } else if (soundType == Sound::SoundType::LOCALIZABLE) {
                sound = std::make_shared<LocalizableSound>(soundFilename, soundCategory, initialVolume, Point3(0.0f, 0.0f, 0.0f), 10.0f);
            } else {
                throw std::invalid_argument("Unknown the sound type to create a new sound: " + std::to_string((int)soundType));
            }

            QVariant variant = soundTriggerTypeComboBox->currentData();
            auto triggerType = static_cast<SoundTrigger::TriggerType>(variant.toInt());
            std::shared_ptr<SoundTrigger> soundTrigger;
            if (triggerType == SoundTrigger::AUTO_TRIGGER) {
                soundTrigger = std::make_shared<AutoTrigger>(PlayBehavior::PLAY_ONCE, sound);
            } else if (triggerType == SoundTrigger::MANUAL_TRIGGER) {
                soundTrigger = std::make_shared<ManualTrigger>(PlayBehavior::PLAY_ONCE);
            } else if (triggerType == SoundTrigger::AREA_TRIGGER) {
                auto newDefaultShape = DefaultSoundShapeCreator(*sound).createDefaultSoundShape(SoundShape::SPHERE_SHAPE);
                soundTrigger = std::make_shared<AreaTrigger>(PlayBehavior::PLAY_ONCE, std::move(newDefaultShape));
            } else {
                throw std::invalid_argument("Unknown the trigger type to create a new sound trigger: " + std::to_string(triggerType));
            }

            soundEntity->setSoundComponent(std::make_unique<SoundComponent>(sound, soundTrigger));
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Error", e.what());
            return Rejected;
        }

        return result;
    }

    std::unique_ptr<SoundEntity> NewSoundDialog::moveSoundEntity() {
        return std::move(soundEntity);
    }

    void NewSoundDialog::showSoundFilenameDialog() {
        QString directory = preferredSoundPath.isEmpty() ? QString::fromStdString(FileSystem::instance().getResourcesDirectory()) : preferredSoundPath;
        QString filename = QFileDialog::getOpenFileName(this, tr("Open sound file"), directory, "Sound file (*.ogg)", nullptr, QFileDialog::DontUseNativeDialog);
        if (!filename.isNull()) {
            this->soundFilename = filename.toUtf8().constData();
            this->soundFilenameText->setText(filename);

            std::string preferredSoundPathString = FileUtil::getDirectory(soundFilename);
            preferredSoundPath = QString::fromStdString(preferredSoundPathString);
        }
    }

    void NewSoundDialog::done(int r) {
        if (Accepted == r) {
            bool hasError = false;

            updateSoundName();
            LabelStyleHelper::applyNormalStyle(soundNameLabel);
            LabelStyleHelper::applyNormalStyle(soundFilenameLabel);

            if (soundName.empty()) {
                LabelStyleHelper::applyErrorStyle(soundNameLabel, "Sound name is mandatory");
                hasError = true;
            } else if (isSoundEntityExist(soundName)) {
                LabelStyleHelper::applyErrorStyle(soundNameLabel, "Sound name is already used");
                hasError = true;
            }
            if (soundFilename.empty()) {
                LabelStyleHelper::applyErrorStyle(soundFilenameLabel, "Sound file is mandatory");
                hasError = true;
            } else {
                QVariant soundTypeVariant = soundTypeComboBox->currentData();
                auto soundType = static_cast<Sound::SoundType>(soundTypeVariant.toInt());
                if (soundType == Sound::SoundType::LOCALIZABLE) {
                    SoundFileReader soundFileReader(soundFilename);
                    if (soundFileReader.getNumberOfChannels() != 1) {
                        LabelStyleHelper::applyErrorStyle(soundFilenameLabel, "Localizable sound must be a mono sound");
                        hasError = true;
                    }
                }
            }

            if (!hasError) {
                r = buildSoundEntity(r);
                QDialog::done(r);
            }
        } else {
            QDialog::done(r);
        }
    }

    bool NewSoundDialog::isSoundEntityExist(std::string_view name) const {
        std::list<const SoundEntity*> soundEntities = soundController->getSoundEntities();
        return std::ranges::any_of(soundEntities, [name](const auto& soundEntity){ return soundEntity->getName() == name; });
    }

}
