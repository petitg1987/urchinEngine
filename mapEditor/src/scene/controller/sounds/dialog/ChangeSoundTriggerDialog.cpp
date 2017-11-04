#include <stdexcept>
#include <QtWidgets/QDialogButtonBox>
#include <QVariant>

#include "ChangeSoundTriggerDialog.h"

namespace urchin
{

	ChangeSoundTriggerDialog::ChangeSoundTriggerDialog(QWidget *parent) :
		QDialog(parent)
	{
		this->setWindowTitle("Select Sound Trigger Type");
		this->resize(245, 80);
		this->setFixedSize(this->width(),this->height());

		auto *mainLayout = new QGridLayout(this);
		mainLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

		setupSoundTriggerTypeFields(mainLayout);

		auto *buttonBox = new QDialogButtonBox();
		mainLayout->addWidget(buttonBox, 1, 0, 1, 2, Qt::AlignRight);
		buttonBox->setOrientation(Qt::Horizontal);
		buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

		QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
		QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	}

	void ChangeSoundTriggerDialog::setupSoundTriggerTypeFields(QGridLayout *mainLayout)
	{
		soundTriggerTypeLabel = new QLabel("Trigger Type:");
		mainLayout->addWidget(soundTriggerTypeLabel, 0, 0);

		soundTriggerTypeComboBox = new QComboBox();
		mainLayout->addWidget(soundTriggerTypeComboBox, 0, 1);
		soundTriggerTypeComboBox->setFixedWidth(150);
		soundTriggerTypeComboBox->addItem(MANUAL_TRIGGER_LABEL, QVariant(SoundTrigger::TriggerType::MANUAL_TRIGGER));
		soundTriggerTypeComboBox->addItem(SHAPE_TRIGGER_LABEL, QVariant(SoundTrigger::TriggerType::SHAPE_TRIGGER));
	}

	void ChangeSoundTriggerDialog::done(int r)
	{
		if(QDialog::Accepted == r)
		{
			QVariant variant = soundTriggerTypeComboBox->currentData();
			triggerType = static_cast<SoundTrigger::TriggerType>(variant.toInt());
			QDialog::done(r);
		}else
		{
			QDialog::done(r);
		}
	}

	SoundTrigger::TriggerType ChangeSoundTriggerDialog::getTriggerType() const
	{
		return triggerType;
	}

}
