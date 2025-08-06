#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFileDialog>

#include "widget/style/LabelStyleHelper.h"
#include "widget/style/ButtonStyleHelper.h"
#include "panel/object/dialog/ChangeSoundDialog.h"

namespace urchin {

    //static
    QString ChangeSoundDialog::preferredSoundPath = QString();

    ChangeSoundDialog::ChangeSoundDialog(QWidget* parent) :
                QDialog(parent),
                soundTypeComboBox(nullptr),
                soundType(std::nullopt) {
        this->setWindowTitle("Change Sound");
        this->resize(530, 120);
        this->setFixedSize(this->width(), this->height());

        auto* mainLayout = new QGridLayout(this);
        mainLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        setupSoundTypeFields(mainLayout);
        setupSoundFilenameFields(mainLayout);

        auto* buttonBox = new QDialogButtonBox();
        mainLayout->addWidget(buttonBox, 2, 0, 1, 3, Qt::AlignRight);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

        connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    }

    void ChangeSoundDialog::setupSoundTypeFields(QGridLayout* mainLayout) {
        auto *soundTypeLabel = new QLabel("Sound Type:");
        mainLayout->addWidget(soundTypeLabel, 0, 0);

        soundTypeComboBox = new QComboBox();
        mainLayout->addWidget(soundTypeComboBox, 0, 1);
        soundTypeComboBox->setFixedWidth(150);
        soundTypeComboBox->addItem(NONE_LABEL, QVariant(-1));
        soundTypeComboBox->addItem(LOCALIZABLE_SOUND_LABEL, QVariant((int)Sound::SoundType::LOCALIZABLE));
        soundTypeComboBox->addItem(GLOBAL_SOUND_LABEL, QVariant((int)Sound::SoundType::GLOBAL));
    }

    void ChangeSoundDialog::setupSoundFilenameFields(QGridLayout* mainLayout) {
        soundFilenameLabel = new QLabel("Sound File:");
        mainLayout->addWidget(soundFilenameLabel, 1, 0);

        soundFilenameText = new QLineEdit();
        mainLayout->addWidget(soundFilenameText, 1, 1);
        soundFilenameText->setReadOnly(true);
        soundFilenameText->setFixedWidth(360);

        auto* selectSoundFileButton = new QPushButton("...");
        mainLayout->addWidget(selectSoundFileButton, 1, 2);
        ButtonStyleHelper::applyDefaultStyle(selectSoundFileButton);
        selectSoundFileButton->setFixedWidth(22);
        connect(selectSoundFileButton, SIGNAL(clicked()), this, SLOT(showSoundFilenameDialog()));
    }

    void ChangeSoundDialog::showSoundFilenameDialog() {
        QString directory = preferredSoundPath.isEmpty() ? QString::fromStdString(FileSystem::instance().getResourcesDirectory()) : preferredSoundPath;
        QString filename = QFileDialog::getOpenFileName(this, tr("Open sound file"), directory, "Sound file (*.ogg)", nullptr, QFileDialog::DontUseNativeDialog);
        if (!filename.isNull()) {
            this->soundFilename = filename.toStdString();
            this->soundFilenameText->setText(filename);

            std::string preferredSoundPathString = FileUtil::getDirectory(soundFilename);
            preferredSoundPath = QString::fromStdString(preferredSoundPathString);
        }
    }

    void ChangeSoundDialog::done(int r) {
        if (Accepted == r) {
            bool hasError = false;

            LabelStyleHelper::resetErrorStyle(soundFilenameLabel);

            int soundTypeIndex = soundTypeComboBox->currentData().toInt();
            if (soundFilename.empty() && soundTypeIndex != -1) {
                LabelStyleHelper::applyErrorStyle(soundFilenameLabel, "Sound file is mandatory");
                hasError = true;
            } else {
                auto soundType = static_cast<Sound::SoundType>(soundTypeIndex);
                if (soundType == Sound::SoundType::LOCALIZABLE) {
                    SoundFileReader soundFileReader(soundFilename);
                    if (soundFileReader.getNumberOfChannels() != 1) {
                        LabelStyleHelper::applyErrorStyle(soundFilenameLabel, "Localizable sound must be a mono sound");
                        hasError = true;
                    }
                }
            }

            if (!hasError) {
                if (soundTypeIndex == -1) {
                    soundType = std::nullopt;
                } else {
                    soundType = static_cast<Sound::SoundType>(soundTypeIndex);
                }
                QDialog::done(r);
            }
        } else {
            QDialog::done(r);
        }
    }

    std::optional<Sound::SoundType> ChangeSoundDialog::getSoundType() const {
        return soundType;
    }

    std::string ChangeSoundDialog::getSoundFilename() const {
        return soundFilename;
    }

}
