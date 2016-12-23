#include <stdexcept>
#include <QtWidgets/QDialogButtonBox>
#include <QVariant>

#include "UrchinCommon.h"
#include "ChangeSoundTriggerDialog.h"

namespace urchin
{

	ChangeSoundTriggerDialog::ChangeSoundTriggerDialog(QWidget *parent) :
		QDialog(parent)
	{
		this->setWindowTitle("Select Sound Trigger Type");
		this->resize(290, 100);

		setupSoundTriggerTypeFields();

		QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
		buttonBox->setGeometry(QRect(30, 60, 245, 22));
		buttonBox->setOrientation(Qt::Horizontal);
		buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

		QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
		QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	}

	void ChangeSoundTriggerDialog::setupSoundTriggerTypeFields()
	{
		soundTriggerTypeLabel = new QLabel("Trigger Type:", this);
		soundTriggerTypeLabel->setGeometry(QRect(10, 20, 90, 22));

		soundTriggerTypeComboBox = new QComboBox(this);
		soundTriggerTypeComboBox->setGeometry(QRect(100, 20, 175, 22));
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
