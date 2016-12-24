#include <QtWidgets/QLabel>

#include "BodySphereShapeWidget.h"
#include "support/SpinBoxStyleHelper.h"

namespace urchin
{

	BodySphereShapeWidget::BodySphereShapeWidget(const SceneObject *sceneObject) :
			BodyShapeWidget(sceneObject)
	{
		QLabel *radiusLabel = new QLabel("Radius:");
		mainLayout->addWidget(radiusLabel, 0, 0);

		radius = new QDoubleSpinBox();
		mainLayout->addWidget(radius, 0, 1);
		SpinBoxStyleHelper::applyDefaultStyleOn(radius);
		radius->setMinimum(0.0);
		connect(radius, SIGNAL(valueChanged(double)), this, SLOT(updateBodyShape()));
	}

	BodySphereShapeWidget::~BodySphereShapeWidget()
	{

	}

	std::string BodySphereShapeWidget::getBodyShapeName() const
	{
		return SPHERE_SHAPE_LABEL;
	}

	void BodySphereShapeWidget::doSetupShapePropertiesFrom(std::shared_ptr<const CollisionShape3D> shape)
	{
		const CollisionSphereShape *sphereShape = static_cast<const CollisionSphereShape *>(shape.get());

		radius->setValue(sphereShape->getRadius());
	}

	std::shared_ptr<const CollisionShape3D> BodySphereShapeWidget::createBodyShape() const
	{
		return std::make_shared<const CollisionSphereShape>(radius->value());
	}
}
