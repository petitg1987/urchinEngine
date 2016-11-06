#include <QtWidgets/QLabel>

#include "SoundSphereShapeWidget.h"
#include "support/SpinBoxStyleHelper.h"

namespace urchin
{

	SoundSphereShapeWidget::SoundSphereShapeWidget(QWidget *parent, const SceneSound *sceneSound) :
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

		QLabel *radiusLabel = new QLabel(this);
		radiusLabel->setText("Radius:");
		radiusLabel->setGeometry(QRect(5, 50, 80, 22));

		radius = new QDoubleSpinBox(this);
		radius->setGeometry(QRect(85, 50, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(radius);
		radius->setMinimum(0.0);
		connect(radius, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));
	}

	SoundSphereShapeWidget::~SoundSphereShapeWidget()
	{

	}

	std::string SoundSphereShapeWidget::getSoundShapeName() const
	{
		return SPHERE_SHAPE_LABEL;
	}

	void SoundSphereShapeWidget::doSetupShapePropertiesFrom(const SoundShape *shape)
	{
		const SoundSphere *sphereShape = static_cast<const SoundSphere *>(shape);

		positionX->setValue(sphereShape->getPosition().X);
		positionY->setValue(sphereShape->getPosition().Y);
		positionZ->setValue(sphereShape->getPosition().Z);

		radius->setValue(sphereShape->getRadius());
	}

	const SoundShape *SoundSphereShapeWidget::createSoundShape() const
	{
		Point3<float> position(positionX->value(), positionY->value(), positionZ->value());

		return new SoundSphere(radius->value(), position, getMarginValue());
	}
}
