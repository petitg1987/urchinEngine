#include <stdexcept>
#include <QtWidgets/QDialogButtonBox>
#include <QVariant>

#include "ChangeSoundShapeDialog.h"
#include "scene/controller/sounds/soundshape/SoundShapeWidget.h"

namespace urchin
{

	ChangeSoundShapeDialog::ChangeSoundShapeDialog(QWidget *parent) :
		QDialog(parent)
	{
		this->setWindowTitle("Select Sound Trigger Shape");
		this->resize(245, 80);
		this->setFixedSize(this->width(),this->height());

		auto *mainLayout = new QGridLayout(this);
		mainLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

		setupSoundShapeTypeFields(mainLayout);

		auto *buttonBox = new QDialogButtonBox();
		mainLayout->addWidget(buttonBox, 1, 0, 1, 2, Qt::AlignRight);
		buttonBox->setOrientation(Qt::Horizontal);
		buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

		QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
		QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	}

	void ChangeSoundShapeDialog::setupSoundShapeTypeFields(QGridLayout *mainLayout)
	{
		soundShapeTypeLabel = new QLabel("Shape Type:");
		mainLayout->addWidget(soundShapeTypeLabel, 0, 0);

		soundShapeTypeComboBox = new QComboBox();
		mainLayout->addWidget(soundShapeTypeComboBox, 0, 1);
		soundShapeTypeComboBox->setFixedWidth(150);
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
