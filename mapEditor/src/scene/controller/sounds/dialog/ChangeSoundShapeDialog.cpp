#include <stdexcept>
#include <QtWidgets/QDialogButtonBox>
#include <QVariant>

#include "UrchinCommon.h"
#include "ChangeSoundShapeDialog.h"
#include "scene/controller/sounds/soundshape/SoundShapeWidget.h"

namespace urchin
{

	ChangeSoundShapeDialog::ChangeSoundShapeDialog(QWidget *parent) :
		QDialog(parent)
	{
		this->setWindowTitle("Select Sound Trigger Shape");
		this->resize(290, 100);

		setupSoundShapeTypeFields();

		QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
		buttonBox->setGeometry(QRect(30, 60, 245, 22));
		buttonBox->setOrientation(Qt::Horizontal);
		buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

		QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
		QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	}

	void ChangeSoundShapeDialog::setupSoundShapeTypeFields()
	{
		soundShapeTypeLabel = new QLabel("Shape Type:", this);
		soundShapeTypeLabel->setGeometry(QRect(10, 20, 90, 22));

		soundShapeTypeComboBox = new QComboBox(this);
		soundShapeTypeComboBox->setGeometry(QRect(100, 20, 175, 22));
		soundShapeTypeComboBox->addItem(SPHERE_SHAPE_LABEL, QVariant(SoundShape::ShapeType::SPHERE_SHAPE));
		soundShapeTypeComboBox->addItem(BOX_SHAPE_LABEL, QVariant(SoundShape::ShapeType::BOX_SHAPE));
	}

	void ChangeSoundShapeDialog::done(int r)
	{
		if(QDialog::Accepted == r)
		{
			QVariant variant = soundShapeTypeComboBox->currentData();
			shapeType = static_cast<SoundShape::ShapeType>(variant.toInt());
			QDialog::done(r);
		}else
		{
			QDialog::done(r);
		}
	}

	SoundShape::ShapeType ChangeSoundShapeDialog::getShapeType() const
	{
		return shapeType;
	}

}
