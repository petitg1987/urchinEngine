#include <QtWidgets/QLabel>

#include "SoundBoxShapeWidget.h"
#include "support/SpinBoxStyleHelper.h"

namespace urchin
{

	SoundBoxShapeWidget::SoundBoxShapeWidget(QWidget *parent, const SceneSound *sceneSound) :
			SoundShapeWidget(parent, sceneSound)
	{
		QLabel *positionLabel = new QLabel(this);
		positionLabel->setText("Position:");
		positionLabel->setGeometry(QRect(5, 0, 80, 22));

		positionX = new QDoubleSpinBox(this);
		positionX->setGeometry(QRect(85, 0, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(positionX);
		connect(positionX, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));

		positionY = new QDoubleSpinBox(this);
		positionY->setGeometry(QRect(165, 0, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(positionY);
		connect(positionY, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));

		positionZ = new QDoubleSpinBox(this);
		positionZ->setGeometry(QRect(245, 0, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(positionZ);
		connect(positionZ, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));

		QLabel *halfSizesLabel = new QLabel(this);
		halfSizesLabel->setText("Half Sizes:");
		halfSizesLabel->setGeometry(QRect(5, 25, 80, 22));

		halfSizeX = new QDoubleSpinBox(this);
		halfSizeX->setGeometry(QRect(85, 25, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(halfSizeX);
		connect(halfSizeX, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));

		halfSizeY = new QDoubleSpinBox(this);
		halfSizeY->setGeometry(QRect(165, 25, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(halfSizeY);
		connect(halfSizeY, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));

		halfSizeZ = new QDoubleSpinBox(this);
		halfSizeZ->setGeometry(QRect(245, 25, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(halfSizeZ);
		connect(halfSizeZ, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));

		QLabel *orientationAxisLabel = new QLabel(this);
		orientationAxisLabel->setText("Orient. Axis:");
		orientationAxisLabel->setGeometry(QRect(5, 50, 80, 22));

		orientationAxisX = new QDoubleSpinBox(this);
		orientationAxisX->setGeometry(QRect(85, 50, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(orientationAxisX);
		connect(orientationAxisX, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));

		orientationAxisY = new QDoubleSpinBox(this);
		orientationAxisY->setGeometry(QRect(165, 50, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(orientationAxisY);
		connect(orientationAxisY, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));

		orientationAxisZ = new QDoubleSpinBox(this);
		orientationAxisZ->setGeometry(QRect(245, 50, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(orientationAxisZ);
		connect(orientationAxisZ, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));

		QLabel *orientationAngleLabel = new QLabel(this);
		orientationAngleLabel->setText("Orient. Angle:");
		orientationAngleLabel->setGeometry(QRect(5, 75, 80, 22));

		orientationAngle = new QDoubleSpinBox(this);
		orientationAngle->setGeometry(QRect(85, 75, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(orientationAngle);
		orientationAngle->setSingleStep(5.0);
		orientationAngle->setMinimum(0.0);
		orientationAngle->setMaximum(360.0);
		connect(orientationAngle, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));

		QLabel *marginLabel = new QLabel(this);
		marginLabel->setText("Margin:");
		marginLabel->setGeometry(QRect(5, 100, 80, 22));

		margin = new QDoubleSpinBox(this);
		margin->setGeometry(QRect(85, 100, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(margin);
		margin->setMinimum(0.0);
		connect(margin, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));
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

		Vector3<float> orientationAxis;
		float orientationAngleRadian;
		boxShape->getOrientation().toAxisAngle(orientationAxis, orientationAngleRadian);
		double orientationAngleDegree = AngleConverter<double>::toDegree(orientationAngleRadian);
		orientationAxisX->setValue(orientationAxis.X);
		orientationAxisY->setValue(orientationAxis.Y);
		orientationAxisZ->setValue(orientationAxis.Z);
		orientationAngle->setValue(orientationAngleDegree);

		margin->setValue(boxShape->getMargin());
	}

	const SoundShape *SoundBoxShapeWidget::createSoundShape() const
	{
		Point3<float> position(positionX->value(), positionY->value(), positionZ->value());

		Vector3<float> halfSizes(halfSizeX->value(), halfSizeY->value(), halfSizeZ->value());

		double orientationAngleDegree = orientationAngle->value();
		double orientationAngleRadian = AngleConverter<double>::toRadian(orientationAngleDegree);
		Vector3<float> orientationAxis(orientationAxisX->value(), orientationAxisY->value(), orientationAxisZ->value());
		Quaternion<float> orientation(orientationAxis, orientationAngleRadian);

		return new SoundBox(halfSizes, position, orientation, margin->value());
	}
}
