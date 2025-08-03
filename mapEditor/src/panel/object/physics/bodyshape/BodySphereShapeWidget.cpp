#include "panel/object/physics/bodyshape/BodySphereShapeWidget.h"
#include "widget/style/SpinBoxStyleHelper.h"

namespace urchin {

    BodySphereShapeWidget::BodySphereShapeWidget(const ObjectEntity* objectEntity) :
            BodyShapeWidget(objectEntity) {
        auto* radiusLabel = new QLabel("Radius:");
        mainLayout->addWidget(radiusLabel, 0, 0);

        radius = new QDoubleSpinBox();
        mainLayout->addWidget(radius, 0, 1);
        SpinBoxStyleHelper::applyDefaultStyleOn(radius, 0.001);
        connect(radius, SIGNAL(valueChanged(double)), this, SLOT(updateBodyShape()));
    }

    std::string BodySphereShapeWidget::getBodyShapeName() const {
        return SPHERE_SHAPE_LABEL;
    }

    void BodySphereShapeWidget::doSetupShapePropertiesFrom(const CollisionShape3D& shape) {
        const auto& sphereShape = static_cast<const CollisionSphereShape&>(shape);

        radius->setValue(sphereShape.getRadius());
    }

    std::unique_ptr<const CollisionShape3D> BodySphereShapeWidget::createBodyShape() const {
        return std::make_unique<const CollisionSphereShape>(radius->value());
    }
}
