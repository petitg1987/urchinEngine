#include <QtWidgets/QLabel>

#include "BodyBoxShapeWidget.h"
#include "widget/style/SpinBoxStyleHelper.h"

namespace urchin
{

    BodyBoxShapeWidget::BodyBoxShapeWidget(const SceneObject *sceneObject) :
            BodyShapeWidget(sceneObject)
    {
        auto *halfSizesLabel = new QLabel("Half Sizes:", this);
        mainLayout->addWidget(halfSizesLabel, 0, 0);

        auto *halfSizeLayout = new QHBoxLayout();
        mainLayout->addLayout(halfSizeLayout, 0, 1);
        halfSizeX = new QDoubleSpinBox();
        halfSizeLayout->addWidget(halfSizeX);
        SpinBoxStyleHelper::applyDefaultStyleOn(halfSizeX);
        halfSizeX->setMinimum(0.0);
        connect(halfSizeX, SIGNAL(valueChanged(double)), this, SLOT(updateBodyShape()));
        halfSizeY = new QDoubleSpinBox();
        halfSizeLayout->addWidget(halfSizeY);
        SpinBoxStyleHelper::applyDefaultStyleOn(halfSizeY);
        halfSizeY->setMinimum(0.0);
        connect(halfSizeY, SIGNAL(valueChanged(double)), this, SLOT(updateBodyShape()));
        halfSizeZ = new QDoubleSpinBox();
        halfSizeLayout->addWidget(halfSizeZ);
        SpinBoxStyleHelper::applyDefaultStyleOn(halfSizeZ);
        halfSizeZ->setMinimum(0.0);
        connect(halfSizeZ, SIGNAL(valueChanged(double)), this, SLOT(updateBodyShape()));
    }

    std::string BodyBoxShapeWidget::getBodyShapeName() const
    {
        return BOX_SHAPE_LABEL;
    }

    void BodyBoxShapeWidget::doSetupShapePropertiesFrom(std::shared_ptr<const CollisionShape3D> shape)
    {
        auto *boxShape = dynamic_cast<const CollisionBoxShape *>(shape.get());

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
