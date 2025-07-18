#include <panel/object/physics/bodyshape/BodyCapsuleShapeWidget.h>
#include <widget/style/SpinBoxStyleHelper.h>

namespace urchin {

    BodyCapsuleShapeWidget::BodyCapsuleShapeWidget(const ObjectEntity* objectEntity) :
            BodyShapeWidget(objectEntity) {
        auto* radiusLabel = new QLabel("Radius:");
        mainLayout->addWidget(radiusLabel, 0, 0);

        radius = new QDoubleSpinBox();
        mainLayout->addWidget(radius, 0, 1);
        SpinBoxStyleHelper::applyDefaultStyleOn(radius);
        radius->setMinimum(0.001);
        connect(radius, SIGNAL(valueChanged(double)), this, SLOT(updateBodyShape()));

        auto* cylinderHeightLabel = new QLabel("Cylinder Height:");
        mainLayout->addWidget(cylinderHeightLabel, 1, 0);

        cylinderHeight = new QDoubleSpinBox();
        mainLayout->addWidget(cylinderHeight, 1, 1);
        SpinBoxStyleHelper::applyDefaultStyleOn(cylinderHeight);
        cylinderHeight->setMinimum(0.001);
        connect(cylinderHeight, SIGNAL(valueChanged(double)), this, SLOT(updateBodyShape()));

        auto* orientationLabel = new QLabel("Orientation:");
        mainLayout->addWidget(orientationLabel, 2, 0);

        orientation = new QComboBox();
        mainLayout->addWidget(orientation, 2, 1);
        orientation->addItem("X", QVariant(CapsuleShape<float>::CapsuleOrientation::CAPSULE_X));
        orientation->addItem("Y", QVariant(CapsuleShape<float>::CapsuleOrientation::CAPSULE_Y));
        orientation->addItem("Z", QVariant(CapsuleShape<float>::CapsuleOrientation::CAPSULE_Z));
        connect(orientation, SIGNAL(currentIndexChanged(int)), this, SLOT(updateCapsuleOrientation()));
    }

    std::string BodyCapsuleShapeWidget::getBodyShapeName() const {
        return CAPSULE_SHAPE_LABEL;
    }

    void BodyCapsuleShapeWidget::updateCapsuleOrientation() {
        std::map<std::string, std::string, std::less<>> params{{"axis", TypeConverter::toString(orientation->currentData().toInt())}};
        std::unique_ptr<const CollisionShape3D> newShape = DefaultBodyShapeGenerator(*getObjectEntity(), DefaultShapeQuality::LOW).generate(CollisionShape3D::ShapeType::CAPSULE_SHAPE, params);
        const auto& capsuleShape = static_cast<const CollisionCapsuleShape&>(*newShape);

        disableShapeEvents(true);
        radius->setValue(capsuleShape.getRadius());
        cylinderHeight->setValue(capsuleShape.getCylinderHeight());
        disableShapeEvents(false);

        updateBodyShape();
    }

    void BodyCapsuleShapeWidget::disableShapeEvents(bool disable) const {
        radius->blockSignals(disable);
        cylinderHeight->blockSignals(disable);
        orientation->blockSignals(disable);
    }

    void BodyCapsuleShapeWidget::doSetupShapePropertiesFrom(const CollisionShape3D& shape) {
        const auto& capsuleShape = static_cast<const CollisionCapsuleShape&>(shape);

        disableShapeEvents(true); //required for compound shape to not re-call updateCapsuleOrientation()
        radius->setValue(capsuleShape.getRadius());
        cylinderHeight->setValue(capsuleShape.getCylinderHeight());
        int index = orientation->findData(capsuleShape.getCapsuleOrientation());
        if (index != -1) {
            orientation->setCurrentIndex(index);
        }
        disableShapeEvents(false);
    }

    std::unique_ptr<const CollisionShape3D> BodyCapsuleShapeWidget::createBodyShape() const {
        QVariant variant = orientation->currentData();
        auto orientation = static_cast<CapsuleShape<float>::CapsuleOrientation>(variant.toInt());

        return std::make_unique<const CollisionCapsuleShape>(radius->value(), cylinderHeight->value(), orientation);
    }
}
