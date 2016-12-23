#include <QtWidgets/QLabel>

#include "SoundBoxShapeWidget.h"
#include "support/SpinBoxStyleHelper.h"
#include "support/ComboBoxStyleHelper.h"

namespace urchin
{

	SoundBoxShapeWidget::SoundBoxShapeWidget(const SceneSound *sceneSound) :
			SoundShapeWidget(sceneSound)
	{
		QLabel *positionLabel = new QLabel("Position:", this);
		mainLayout->addWidget(positionLabel, 1, 0);

		QHBoxLayout *positionLayout = new QHBoxLayout();
		mainLayout->addLayout(positionLayout, 1, 1);
		positionX = new QDoubleSpinBox();
		positionLayout->addWidget(positionX);
		SpinBoxStyleHelper::applyDefaultStyleOn(positionX);
		connect(positionX, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));
		positionY = new QDoubleSpinBox();
		positionLayout->addWidget(positionY);
		SpinBoxStyleHelper::applyDefaultStyleOn(positionY);
		connect(positionY, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));
		positionZ = new QDoubleSpinBox();
		positionLayout->addWidget(positionZ);
		SpinBoxStyleHelper::applyDefaultStyleOn(positionZ);
		connect(positionZ, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));

		QLabel *halfSizesLabel = new QLabel("Half Sizes:");
		mainLayout->addWidget(halfSizesLabel, 2, 0);

		QHBoxLayout *halfSizeLayout = new QHBoxLayout();
		mainLayout->addLayout(halfSizeLayout, 2, 1);
		halfSizeX = new QDoubleSpinBox();
		halfSizeLayout->addWidget(halfSizeX);
		SpinBoxStyleHelper::applyDefaultStyleOn(halfSizeX);
		connect(halfSizeX, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));
		halfSizeY = new QDoubleSpinBox();
		halfSizeLayout->addWidget(halfSizeY);
		SpinBoxStyleHelper::applyDefaultStyleOn(halfSizeY);
		connect(halfSizeY, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));
		halfSizeZ = new QDoubleSpinBox();
		halfSizeLayout->addWidget(halfSizeZ);
		SpinBoxStyleHelper::applyDefaultStyleOn(halfSizeZ);
		connect(halfSizeZ, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));

		QLabel *orientationTypeLabel = new QLabel("Orient. Type:", this);
		mainLayout->addWidget(orientationTypeLabel, 3, 0);

		orientationType = new QComboBox();
		mainLayout->addWidget(orientationType, 3, 1);
		ComboBoxStyleHelper::applyOrientationStyleOn(orientationType);
		connect(orientationType, SIGNAL(currentIndexChanged(int)), this, SLOT(updateShapeOrientationType()));

		QLabel *eulerAngleLabel = new QLabel("Euler Angle:", this);
		mainLayout->addWidget(eulerAngleLabel, 4, 0);

		QHBoxLayout *eulerLayout = new QHBoxLayout();
		mainLayout->addLayout(eulerLayout, 4, 1);
		eulerAxis0 = new QDoubleSpinBox();
		eulerLayout->addWidget(eulerAxis0);
		SpinBoxStyleHelper::applyAngleStyleOn(eulerAxis0);
		connect(eulerAxis0, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));
		eulerAxis1 = new QDoubleSpinBox();
		eulerLayout->addWidget(eulerAxis1);
		SpinBoxStyleHelper::applyAngleStyleOn(eulerAxis1);
		connect(eulerAxis1, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));
		eulerAxis2 = new QDoubleSpinBox();
		eulerLayout->addWidget(eulerAxis2);
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
