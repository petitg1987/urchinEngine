#include <QtWidgets/QLabel>

#include "BodyCapsuleShapeWidget.h"
#include "support/SpinBoxStyleHelper.h"

namespace urchin
{

	BodyCapsuleShapeWidget::BodyCapsuleShapeWidget(QWidget *parent, const SceneObject *sceneObject) :
			BodyShapeWidget(parent, sceneObject)
	{
		QLabel *radiusLabel = new QLabel("Radius:", this);
		radiusLabel->setGeometry(QRect(5, 0, 80, 22));

		radius = new QDoubleSpinBox(this);
		radius->setGeometry(QRect(85, 0, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(radius);
		radius->setMinimum(0.0);
		connect(radius, SIGNAL(valueChanged(double)), this, SLOT(updateBodyShape()));

		QLabel *cylinderHeightLabel = new QLabel("Cylinder Height:", this);
		cylinderHeightLabel->setGeometry(QRect(5, 25, 80, 22));

		cylinderHeight = new QDoubleSpinBox(this);
		cylinderHeight->setGeometry(QRect(85, 25, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(cylinderHeight);
		cylinderHeight->setMinimum(0.0);
		connect(cylinderHeight, SIGNAL(valueChanged(double)), this, SLOT(updateBodyShape()));

		QLabel *orientationLabel = new QLabel("Orientation:", this);
		orientationLabel->setGeometry(QRect(5, 50, 80, 22));

		orientation = new QComboBox(this);
		orientation->setGeometry(QRect(85, 50, 80, 22));
		orientation->addItem("X", QVariant(CapsuleShape<float>::CapsuleOrientation::CAPSULE_X));
		orientation->addItem("Y", QVariant(CapsuleShape<float>::CapsuleOrientation::CAPSULE_Y));
		orientation->addItem("Z", QVariant(CapsuleShape<float>::CapsuleOrientation::CAPSULE_Z));
		connect(orientation, SIGNAL(currentIndexChanged(int)), this, SLOT(updateBodyShape()));
	}

	BodyCapsuleShapeWidget::~BodyCapsuleShapeWidget()
	{

	}

	std::string BodyCapsuleShapeWidget::getBodyShapeName() const
	{
		return CAPSULE_SHAPE_LABEL;
	}

	void BodyCapsuleShapeWidget::doSetupShapePropertiesFrom(std::shared_ptr<const CollisionShape3D> shape)
	{
		const CollisionCapsuleShape *capsuleShape = static_cast<const CollisionCapsuleShape *>(shape.get());

		radius->setValue(capsuleShape->getRadius());
		cylinderHeight->setValue(capsuleShape->getCylinderHeight());

		int index = orientation->findData(capsuleShape->getCapsuleOrientation());
		if(index!=-1)
		{
			orientation->setCurrentIndex(index);
		}
	}

	std::shared_ptr<const CollisionShape3D> BodyCapsuleShapeWidget::createBodyShape() const
	{
		QVariant variant = orientation->currentData();
		CapsuleShape<float>::CapsuleOrientation orientation = static_cast<CapsuleShape<float>::CapsuleOrientation>(variant.toInt());

		return std::make_shared<const CollisionCapsuleShape>(radius->value(), cylinderHeight->value(), orientation);
	}
}
