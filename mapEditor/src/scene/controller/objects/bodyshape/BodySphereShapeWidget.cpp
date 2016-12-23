#include <QtWidgets/QLabel>

#include "BodySphereShapeWidget.h"
#include "support/SpinBoxStyleHelper.h"

namespace urchin
{

	BodySphereShapeWidget::BodySphereShapeWidget(QWidget *parent, const SceneObject *sceneObject) :
			BodyShapeWidget(parent, sceneObject)
	{
		QLabel *radiusLabel = new QLabel("Radius:", this);
		radiusLabel->setGeometry(QRect(5, 0, 80, 22));

		radius = new QDoubleSpinBox(this);
		radius->setGeometry(QRect(85, 0, 80, 22));
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
