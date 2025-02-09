#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>

#include <panel/objects/dialog/ChangeSoundTypeDialog.h>

namespace urchin {

    ChangeSoundTypeDialog::ChangeSoundTypeDialog(QWidget* parent) :
                QDialog(parent),
                soundTypeComboBox(nullptr),
                soundType(std::nullopt) {
        this->setWindowTitle("Change Sound Type");
        this->resize(245, 80);
        this->setFixedSize(this->width(), this->height());

        auto* mainLayout = new QGridLayout(this);
        mainLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        setupSoundTypeFields(mainLayout);

        auto* buttonBox = new QDialogButtonBox();
        mainLayout->addWidget(buttonBox, 2, 0, 1, 2, Qt::AlignRight);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

        connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    }

    void ChangeSoundTypeDialog::setupSoundTypeFields(QGridLayout* mainLayout) {
        auto *soundTypeLabel = new QLabel("Sound Type:");
        mainLayout->addWidget(soundTypeLabel, 1, 0);

        soundTypeComboBox = new QComboBox();
        mainLayout->addWidget(soundTypeComboBox, 1, 1);
        soundTypeComboBox->setFixedWidth(150);
        soundTypeComboBox->addItem(NONE_LABEL, QVariant(-1));
        soundTypeComboBox->addItem(LOCALIZABLE_SOUND_LABEL, QVariant((int)Sound::SoundType::LOCALIZABLE));
        soundTypeComboBox->addItem(GLOBAL_SOUND_LABEL, QVariant((int)Sound::SoundType::GLOBAL));
    }

    void ChangeSoundTypeDialog::done(int r) {
        if (Accepted == r) {
            int soundTypeIndex = soundTypeComboBox->currentData().toInt();
            if (soundTypeIndex == -1) {
                soundType = std::nullopt;
            } else {
                soundType = static_cast<Sound::SoundType>(soundTypeIndex);
            }
            QDialog::done(r);
        } else {
            QDialog::done(r);
        }
    }

    std::optional<Sound::SoundType> ChangeSoundTypeDialog::getSoundType() const {
        return soundType;
    }

}
