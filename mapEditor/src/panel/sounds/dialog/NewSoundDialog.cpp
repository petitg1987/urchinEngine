#include <stdexcept>
#include <QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFileDialog>

#include <UrchinCommon.h>
#include <panel/sounds/dialog/NewSoundDialog.h>
#include <widget/style/LabelStyleHelper.h>
#include <widget/style/ButtonStyleHelper.h>

namespace urchin {

    QString NewSoundDialog::preferredSoundPath = QString();

    NewSoundDialog::NewSoundDialog(QWidget* parent, const SoundController* soundController) :
            QDialog(parent),
            soundController(soundController),
            soundNameLabel(nullptr),
            soundNameText(nullptr),
            soundFilenameLabel(nullptr),
            soundFilenameText(nullptr),
            soundTypeLabel(nullptr),
            soundTypeComboBox(nullptr),
            soundCategoryLabel(nullptr),
            soundCategoryComboBox(nullptr),
            sceneSound(nullptr) {
        this->setWindowTitle("New Sound");
        this->resize(530, 160);
        this->setFixedSize(this->width(),this->height());

        auto* mainLayout = new QGridLayout(this);
        mainLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        setupNameFields(mainLayout);
        setupSoundFilenameFields(mainLayout);
        setupSoundTypeFields(mainLayout);
        setupSoundCategoryFields(mainLayout);

        auto* buttonBox = new QDialogButtonBox();
        mainLayout->addWidget(buttonBox, 4, 0, 1, 3, Qt::AlignRight);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

        QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
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
        soundTypeLabel = new QLabel("Sound Type:");
        mainLayout->addWidget(soundTypeLabel, 2, 0);

        soundTypeComboBox = new QComboBox();
        mainLayout->addWidget(soundTypeComboBox, 2, 1);
        soundTypeComboBox->setFixedWidth(150);
        soundTypeComboBox->addItem(GLOBAL_SOUND_LABEL, QVariant(Sound::SoundType::GLOBAL));
        soundTypeComboBox->addItem(SPATIAL_SOUND_LABEL, QVariant(Sound::SoundType::SPATIAL));
    }

    void NewSoundDialog::setupSoundCategoryFields(QGridLayout* mainLayout) {
        soundCategoryLabel = new QLabel("Sound Category:");
        mainLayout->addWidget(soundCategoryLabel, 3, 0);

        soundCategoryComboBox = new QComboBox();
        mainLayout->addWidget(soundCategoryComboBox, 3, 1);
        soundCategoryComboBox->setFixedWidth(150);
        soundCategoryComboBox->addItem(MUSIC_SOUND_LABEL, QVariant(Sound::SoundCategory::MUSIC));
        soundCategoryComboBox->addItem(EFFECTS_SOUND_LABEL, QVariant(Sound::SoundCategory::EFFECTS));
    }

    void NewSoundDialog::updateSoundName() {
        QString soundName = soundNameText->text();
        if (!soundName.isEmpty()) {
            this->soundName = soundName.toUtf8().constData();
        }
    }

    int NewSoundDialog::buildSceneSound(int result) {
        try {
            sceneSound = std::make_unique<SceneSound>();

            sceneSound->setName(soundName);

            std::string resourcesDirectory = FileSystem::instance().getResourcesDirectory();
            std::string relativeSoundFilename = FileUtil::getRelativePath(resourcesDirectory, soundFilename);

            QVariant soundTypeVariant = soundTypeComboBox->currentData();
            auto soundType = static_cast<Sound::SoundType>(soundTypeVariant.toInt());

            QVariant soundCategoryVariant = soundCategoryComboBox->currentData();
            auto soundCategory = static_cast<Sound::SoundCategory>(soundCategoryVariant.toInt());

            std::shared_ptr<Sound> sound;
            if (soundType == Sound::GLOBAL) {
                sound = std::make_shared<GlobalSound>(relativeSoundFilename, soundCategory);
            } else if (soundType == Sound::SPATIAL) {
                sound = std::make_shared<SpatialSound>(relativeSoundFilename, soundCategory, Point3<float>(0.0, 0.0, 0.0));
            } else {
                throw std::invalid_argument("Unknown the sound type to create a new sound: " + std::to_string(soundType));
            }

            auto soundTrigger = std::make_shared<ManualTrigger>(SoundTrigger::PLAY_ONCE);

            sceneSound->setSoundElements(sound, soundTrigger);
        } catch (std::exception& e) {
            QMessageBox::critical(this, "Error", e.what());
            return QDialog::Rejected;
        }

        return result;
    }


    std::unique_ptr<SceneSound> NewSoundDialog::moveSceneSound() {
        return std::move(sceneSound);
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
        if (QDialog::Accepted == r) {
            bool hasError = false;

            updateSoundName();
            LabelStyleHelper::applyNormalStyle(soundNameLabel);
            LabelStyleHelper::applyNormalStyle(soundFilenameLabel);

            if (soundName.empty()) {
                LabelStyleHelper::applyErrorStyle(soundNameLabel, "Sound name is mandatory");
                hasError = true;
            } else if (isSceneSoundExist(soundName)) {
                LabelStyleHelper::applyErrorStyle(soundNameLabel, "Sound name is already used");
                hasError = true;
            }
            if (soundFilename.empty()) {
                LabelStyleHelper::applyErrorStyle(soundFilenameLabel, "Sound file is mandatory");
                hasError = true;
            }

            if (!hasError) {
                r = buildSceneSound(r);
                QDialog::done(r);
            }
        } else {
            QDialog::done(r);
        }
    }

    bool NewSoundDialog::isSceneSoundExist(const std::string& name) {
        std::list<const SceneSound*> sceneSounds = soundController->getSceneSounds();
        return std::any_of(sceneSounds.begin(), sceneSounds.end(), [&name](const auto& ss){return ss->getName() == name;});
    }

}
