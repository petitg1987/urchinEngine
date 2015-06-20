#include <QtWidgets/QLabel>

#include "BodyCylinderShapeWidget.h"
#include "support/SpinBoxStyleHelper.h"

namespace urchin
{

	BodyCylinderShapeWidget::BodyCylinderShapeWidget(QWidget *parent, const SceneObject *sceneObject) :
			BodyShapeWidget(parent, sceneObject)
	{
		QLabel *radiusLabel = new QLabel(this);
		radiusLabel->setText("Radius:");
		radiusLabel->setGeometry(QRect(5, 0, 80, 22));

		radius = new QDoubleSpinBox(this);
		radius->setGeometry(QRect(85, 0, 80, 22));
		SpinBoxStyleHelper::applyNormalStyle(radius);
		radius->setMinimum(0.0);
		connect(radius, SIGNAL(valueChanged(double)), this, SLOT(updateBodyShape()));

		QLabel *heightLabel = new QLabel(this);
		heightLabel->setText("Height:");
		heightLabel->setGeometry(QRect(5, 25, 80, 22));

		height = new QDoubleSpinBox(this);
		height->setGeometry(QRect(85, 25, 80, 22));
		SpinBoxStyleHelper::applyNormalStyle(height);
		height->setMinimum(0.0);
		connect(height, SIGNAL(valueChanged(double)), this, SLOT(updateBodyShape()));

		QLabel *orientationLabel = new QLabel(this);
		orientationLabel->setText("Orientation:");
		orientationLabel->setGeometry(QRect(5, 50, 80, 22));

		orientation = new QComboBox(this);
		orientation->setGeometry(QRect(85, 50, 80, 22));
		orientation->addItem("X", QVariant(CylinderShape<float>::CylinderOrientation::CYLINDER_X));
		orientation->addItem("Y", QVariant(CylinderShape<float>::CylinderOrientation::CYLINDER_Y));
		orientation->addItem("Z", QVariant(CylinderShape<float>::CylinderOrientation::CYLINDER_Z));
		connect(orientation, SIGNAL(currentIndexChanged(int)), this, SLOT(updateBodyShape()));
	}

	BodyCylinderShapeWidget::~BodyCylinderShapeWidget()
	{

	}

	std::string BodyCylinderShapeWidget::getBodyShapeName() const
	{
		return CYLINDER_SHAPE_LABEL;
	}

	void BodyCylinderShapeWidget::doSetupShapePropertiesFrom(std::shared_ptr<const CollisionShape3D> shape)
	{
		const CollisionCylinderShape *cylinderShape = static_cast<const CollisionCylinderShape *>(shape.get());

		radius->setValue(cylinderShape->getRadius());
		height->setValue(cylinderShape->getHeight());

		int index = orientation->findData(cylinderShape->getCylinderOrientation());
		if(index!=-1)
		{
			orientation->setCurrentIndex(index);
		}
	}

	std::shared_ptr<const CollisionShape3D> BodyCylinderShapeWidget::createBodyShape() const
	{
		QVariant variant = orientation->currentData();
		CylinderShape<float>::CylinderOrientation orientation = static_cast<CylinderShape<float>::CylinderOrientation>(variant.toInt());

		return std::make_shared<const CollisionCylinderShape>(radius->value(), height->value(), orientation);
	}
}
