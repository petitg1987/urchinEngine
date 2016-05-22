#include <QtWidgets/QLabel>

#include "BodyBoxShapeWidget.h"
#include "support/SpinBoxStyleHelper.h"

namespace urchin
{

	BodyBoxShapeWidget::BodyBoxShapeWidget(QWidget *parent, const SceneObject *sceneObject) :
			BodyShapeWidget(parent, sceneObject)
	{
		QLabel *halfSizesLabel = new QLabel(this);
		halfSizesLabel->setText("Half Sizes:");
		halfSizesLabel->setGeometry(QRect(5, 0, 80, 22));

		halfSizeX = new QDoubleSpinBox(this);
		halfSizeX->setGeometry(QRect(85, 0, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(halfSizeX);
		halfSizeX->setMinimum(0.0);
		connect(halfSizeX, SIGNAL(valueChanged(double)), this, SLOT(updateBodyShape()));

		halfSizeY = new QDoubleSpinBox(this);
		halfSizeY->setGeometry(QRect(165, 0, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(halfSizeY);
		halfSizeY->setMinimum(0.0);
		connect(halfSizeY, SIGNAL(valueChanged(double)), this, SLOT(updateBodyShape()));

		halfSizeZ = new QDoubleSpinBox(this);
		halfSizeZ->setGeometry(QRect(245, 0, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(halfSizeZ);
		halfSizeZ->setMinimum(0.0);
		connect(halfSizeZ, SIGNAL(valueChanged(double)), this, SLOT(updateBodyShape()));
	}

	BodyBoxShapeWidget::~BodyBoxShapeWidget()
	{

	}

	std::string BodyBoxShapeWidget::getBodyShapeName() const
	{
		return BOX_SHAPE_LABEL;
	}

	void BodyBoxShapeWidget::doSetupShapePropertiesFrom(std::shared_ptr<const CollisionShape3D> shape)
	{
		const CollisionBoxShape *boxShape = static_cast<const CollisionBoxShape *>(shape.get());

		const Vector3<float> &halfSizes = boxShape->getHalfSizes();
		halfSizeX->setValue(halfSizes.X);
		halfSizeY->setValue(halfSizes.Y);
		halfSizeZ->setValue(halfSizes.Z);
	}

	std::shared_ptr<const CollisionShape3D> BodyBoxShapeWidget::createBodyShape() const
	{
		Vector3<float> halfSizes(halfSizeX->value(), halfSizeY->value(), halfSizeZ->value());
		return std::make_shared<const CollisionBoxShape>(halfSizes);
	}
}
