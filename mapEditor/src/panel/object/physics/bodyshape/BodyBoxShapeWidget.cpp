#include <QtWidgets/QLabel>

#include <panel/object/physics/bodyshape/BodyBoxShapeWidget.h>
#include <widget/style/SpinBoxStyleHelper.h>

namespace urchin {

    BodyBoxShapeWidget::BodyBoxShapeWidget(const ObjectEntity* objectEntity) :
            BodyShapeWidget(objectEntity) {
        auto* halfSizesLabel = new QLabel("Half Sizes:", this);
        mainLayout->addWidget(halfSizesLabel, 0, 0);

        auto* halfSizeLayout = new QHBoxLayout();
        mainLayout->addLayout(halfSizeLayout, 0, 1);
        halfSizeX = new QDoubleSpinBox();
        halfSizeLayout->addWidget(halfSizeX);
        SpinBoxStyleHelper::applyDefaultStyleOn(halfSizeX);
        halfSizeX->setMinimum(0.001);
        connect(halfSizeX, SIGNAL(valueChanged(double)), this, SLOT(updateBodyShape()));
        halfSizeY = new QDoubleSpinBox();
        halfSizeLayout->addWidget(halfSizeY);
        SpinBoxStyleHelper::applyDefaultStyleOn(halfSizeY);
        halfSizeY->setMinimum(0.001);
        connect(halfSizeY, SIGNAL(valueChanged(double)), this, SLOT(updateBodyShape()));
        halfSizeZ = new QDoubleSpinBox();
        halfSizeLayout->addWidget(halfSizeZ);
        SpinBoxStyleHelper::applyDefaultStyleOn(halfSizeZ);
        halfSizeZ->setMinimum(0.001);
        connect(halfSizeZ, SIGNAL(valueChanged(double)), this, SLOT(updateBodyShape()));
    }

    std::string BodyBoxShapeWidget::getBodyShapeName() const {
        return BOX_SHAPE_LABEL;
    }

    void BodyBoxShapeWidget::doSetupShapePropertiesFrom(const CollisionShape3D& shape) {
        const auto& boxShape = static_cast<const CollisionBoxShape&>(shape);

        const Vector3<float>& halfSizes = boxShape.getHalfSizes();
        halfSizeX->setValue(halfSizes.X);
        halfSizeY->setValue(halfSizes.Y);
        halfSizeZ->setValue(halfSizes.Z);
    }

    std::unique_ptr<const CollisionShape3D> BodyBoxShapeWidget::createBodyShape() const {
        Vector3 halfSizes((float)halfSizeX->value(), (float)halfSizeY->value(), (float)halfSizeZ->value());
        return std::make_unique<const CollisionBoxShape>(halfSizes);
    }
}
