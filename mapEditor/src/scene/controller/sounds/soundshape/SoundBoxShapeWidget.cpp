#include <QtWidgets/QLabel>

#include "SoundBoxShapeWidget.h"
#include "support/SpinBoxStyleHelper.h"
#include "support/ComboBoxStyleHelper.h"

namespace urchin
{

	SoundBoxShapeWidget::SoundBoxShapeWidget(QWidget *parent, const SceneSound *sceneSound) :
			SoundShapeWidget(parent, sceneSound)
	{
		QLabel *positionLabel = new QLabel(this);
		positionLabel->setText("Position:");
		positionLabel->setGeometry(QRect(5, 25, 80, 22));

		positionX = new QDoubleSpinBox(this);
		positionX->setGeometry(QRect(85, 25, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(positionX);
		connect(positionX, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));

		positionY = new QDoubleSpinBox(this);
		positionY->setGeometry(QRect(165, 25, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(positionY);
		connect(positionY, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));

		positionZ = new QDoubleSpinBox(this);
		positionZ->setGeometry(QRect(245, 25, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(positionZ);
		connect(positionZ, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));

		QLabel *halfSizesLabel = new QLabel(this);
		halfSizesLabel->setText("Half Sizes:");
		halfSizesLabel->setGeometry(QRect(5, 50, 80, 22));

		halfSizeX = new QDoubleSpinBox(this);
		halfSizeX->setGeometry(QRect(85, 50, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(halfSizeX);
		connect(halfSizeX, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));

		halfSizeY = new QDoubleSpinBox(this);
		halfSizeY->setGeometry(QRect(165, 50, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(halfSizeY);
		connect(halfSizeY, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));

		halfSizeZ = new QDoubleSpinBox(this);
		halfSizeZ->setGeometry(QRect(245, 50, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(halfSizeZ);
		connect(halfSizeZ, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));

		QLabel *orientationTypeLabel = new QLabel(this);
		orientationTypeLabel->setText("Orient. Type:");
		orientationTypeLabel->setGeometry(QRect(5, 75, 80, 22));

		orientationType = new QComboBox(this);
		orientationType->setGeometry(QRect(85, 75, 160, 22));
		ComboBoxStyleHelper::applyOrientationStyleOn(orientationType);
		connect(orientationType, SIGNAL(currentIndexChanged(int)), this, SLOT(updateShapeOrientationType()));

		QLabel *eulerAngleLabel = new QLabel(this);
		eulerAngleLabel->setText("Euler Angle:");
		eulerAngleLabel->setGeometry(QRect(5, 100, 80, 22));

		eulerAxis0 = new QDoubleSpinBox(this);
		eulerAxis0->setGeometry(QRect(85, 100, 80, 22));
		SpinBoxStyleHelper::applyAngleStyleOn(eulerAxis0);
		connect(eulerAxis0, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));

		eulerAxis1 = new QDoubleSpinBox(this);
		eulerAxis1->setGeometry(QRect(165, 100, 80, 22));
		SpinBoxStyleHelper::applyAngleStyleOn(eulerAxis1);
		connect(eulerAxis1, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));

		eulerAxis2 = new QDoubleSpinBox(this);
		eulerAxis2->setGeometry(QRect(245, 100, 80, 22));
		SpinBoxStyleHelper::applyAngleStyleOn(eulerAxis2);
		connect(eulerAxis2, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));

	}

	SoundBoxShapeWidget::~SoundBoxShapeWidget()
	{

	}

	std::string SoundBoxShapeWidget::getSoundShapeName() const
	{
		return BOX_SHAPE_LABEL;
	}

	void SoundBoxShapeWidget::doSetupShapePropertiesFrom(const SoundShape *shape)
	{
		const SoundBox *boxShape = static_cast<const SoundBox *>(shape);

		positionX->setValue(boxShape->getCenterPosition().X);
		positionY->setValue(boxShape->getCenterPosition().Y);
		positionZ->setValue(boxShape->getCenterPosition().Z);

		halfSizeX->setValue(boxShape->getHalfSizes().X);
		halfSizeY->setValue(boxShape->getHalfSizes().Y);
		halfSizeZ->setValue(boxShape->getHalfSizes().Z);

		Vector3<float> eulerAngle = boxShape->getOrientation().toEulerAngle(Quaternion<float>::RotationSequence::XYZ);
		this->eulerAxis0->setValue(AngleConverter<double>::toDegree(eulerAngle[0]));
		this->eulerAxis1->setValue(AngleConverter<double>::toDegree(eulerAngle[1]));
		this->eulerAxis2->setValue(AngleConverter<double>::toDegree(eulerAngle[2]));
	}

	void SoundBoxShapeWidget::updateShapeOrientationType()
	{
		if(!disableShapeEvent)
		{
			const SoundBox *boxShape = static_cast<const SoundBox *>(retrieveShape());

			QVariant variant = orientationType->currentData();
			Quaternion<float>::RotationSequence newRotationSequence = static_cast<Quaternion<float>::RotationSequence>(variant.toInt());
			Vector3<float> eulerAngle = boxShape->getOrientation().toEulerAngle(newRotationSequence);

			eulerAxis0->setValue(AngleConverter<float>::toDegree(eulerAngle.X));
			eulerAxis1->setValue(AngleConverter<float>::toDegree(eulerAngle.Y));
			eulerAxis2->setValue(AngleConverter<float>::toDegree(eulerAngle.Z));

			updateSoundShape();
		}
	}

	const SoundShape *SoundBoxShapeWidget::createSoundShape() const
	{
		Point3<float> position(positionX->value(), positionY->value(), positionZ->value());

		Vector3<float> halfSizes(halfSizeX->value(), halfSizeY->value(), halfSizeZ->value());

		Vector3<float> eulerAngle(
			AngleConverter<float>::toRadian(eulerAxis0->value()),
			AngleConverter<float>::toRadian(eulerAxis1->value()),
			AngleConverter<float>::toRadian(eulerAxis2->value())
		);
		QVariant variant = orientationType->currentData();
		Quaternion<float>::RotationSequence rotationSequence = static_cast<Quaternion<float>::RotationSequence>(variant.toInt());
		Quaternion<float> orientation(eulerAngle, rotationSequence);

		return new SoundBox(halfSizes, position, orientation, getMarginValue());
	}
}
