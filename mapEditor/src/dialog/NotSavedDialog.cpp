#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <boost/algorithm/string.hpp>

#include "UrchinCommon.h"
#include "NotSavedDialog.h"

namespace urchin
{

	NotSavedDialog::NotSavedDialog(QWidget *parent) :
		QDialog(parent),
		bNeedSave(false)
	{
		this->setWindowTitle("Save Map");
		this->resize(350, 120);

		QLabel *saveMapLabel = new QLabel(this);
		saveMapLabel->setText("A map is currently open but not saved.\n"
				"Would you like to save it ?");
		saveMapLabel->setGeometry(QRect(10, 20, 400, 50));

		buttonBox = new QDialogButtonBox(this);
		buttonBox->setGeometry(QRect(30, 80, 310, 22));
		buttonBox->setOrientation(Qt::Horizontal);
		buttonBox->setStandardButtons(QDialogButtonBox::Yes|QDialogButtonBox::No|QDialogButtonBox::Cancel);

		QObject::connect(buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttonclick(QAbstractButton*)));
	}

	bool NotSavedDialog::needSave() const
	{
		return bNeedSave;
	}

	void NotSavedDialog::buttonclick(QAbstractButton *button)
	{
		if(buttonBox->standardButton(button) == QDialogButtonBox::Yes)
		{
			bNeedSave = true;
			accept();
		}else if(buttonBox->standardButton(button) == QDialogButtonBox::No)
		{
			bNeedSave = false;
			accept();
		}else if(buttonBox->standardButton(button) == QDialogButtonBox::Cancel)
		{
			bNeedSave = false;
			reject();
		}else
		{
			reject();
		}
	}

}
