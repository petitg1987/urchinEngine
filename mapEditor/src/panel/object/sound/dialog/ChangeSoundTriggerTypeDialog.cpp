#include <QtWidgets/QLabel>
#include <QtWidgets/QDialogButtonBox>

#include "panel/object/sound/dialog/ChangeSoundTriggerTypeDialog.h"

namespace urchin {

    ChangeSoundTriggerTypeDialog::ChangeSoundTriggerTypeDialog(QWidget* parent) :
            QDialog(parent),
            soundTriggerTypeComboBox(nullptr),
            triggerType(SoundTrigger::TriggerType::AUTO_TRIGGER) {
        this->setWindowTitle("Select Trigger Type");
        this->resize(280, 80);
        this->setFixedSize(this->width(), this->height());

        auto* mainLayout = new QGridLayout(this);
        mainLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        setupSoundTriggerTypeFields(mainLayout);

        auto* buttonBox = new QDialogButtonBox();
        mainLayout->addWidget(buttonBox, 1, 0, 1, 2, Qt::AlignRight);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

        connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    }

    void ChangeSoundTriggerTypeDialog::setupSoundTriggerTypeFields(QGridLayout* mainLayout) {
        auto* soundTriggerTypeLabel = new QLabel("Trigger Type:");
        mainLayout->addWidget(soundTriggerTypeLabel, 0, 0);

        soundTriggerTypeComboBox = new QComboBox();
        mainLayout->addWidget(soundTriggerTypeComboBox, 0, 1);
        soundTriggerTypeComboBox->setFixedWidth(150);
        soundTriggerTypeComboBox->addItem(AUTO_TRIGGER_LABEL, QVariant(SoundTrigger::TriggerType::AUTO_TRIGGER));
        soundTriggerTypeComboBox->addItem(MANUAL_TRIGGER_LABEL, QVariant(SoundTrigger::TriggerType::MANUAL_TRIGGER));
        soundTriggerTypeComboBox->addItem(AREA_TRIGGER_LABEL, QVariant(SoundTrigger::TriggerType::AREA_TRIGGER));
    }

    void ChangeSoundTriggerTypeDialog::done(int r) {
        if (Accepted == r) {
            QVariant variant = soundTriggerTypeComboBox->currentData();
            triggerType = static_cast<SoundTrigger::TriggerType>(variant.toInt());
            QDialog::done(r);
        } else {
            QDialog::done(r);
        }
    }

    SoundTrigger::TriggerType ChangeSoundTriggerTypeDialog::getSoundTriggerType() const {
        return triggerType;
    }

}
