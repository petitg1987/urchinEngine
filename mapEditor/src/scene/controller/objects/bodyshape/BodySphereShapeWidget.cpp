#include <QtWidgets/QLabel>

#include "BodySphereShapeWidget.h"
#include "widget/style/SpinBoxStyleHelper.h"

namespace urchin
{

	BodySphereShapeWidget::BodySphereShapeWidget(const SceneObject *sceneObject) :
			BodyShapeWidget(sceneObject)
	{
		auto *radiusLabel = new QLabel("Radius:");
		mainLayout->addWidget(radiusLabel, 0, 0);

		radius = new QDoubleSpinBox();
		mainLayout->addWidget(radius, 0, 1);
		SpinBoxStyleHelper::applyDefaultStyleOn(radius);
		radius->setMinimum(0.0);
		connect(radius, SIGNAL(valueChanged(double)), this, SLOT(updateBodyShape()));
	}

	std::string BodySphereShapeWidget::getBodyShapeName() const
	{
		return SPHERE_SHAPE_LABEL;
	}

	void BodySphereShapeWidget::doSetupShapePropertiesFrom(std::shared_ptr<const CollisionShape3D> shape)
	{
		const auto *sphereShape = dynamic_cast<const CollisionSphereShape *>(shape.get());

		radius->setValue(sphereShape->getRadius());
	}

	std::shared_ptr<const CollisionShape3D> BodySphereShapeWidget::createBodyShape() const
	{
		return std::make_shared<const CollisionSphereShape>(radius->value());
	}
}
