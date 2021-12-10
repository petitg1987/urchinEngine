#include <QtWidgets/QLabel>

#include <panel/models/bodyshape/BodyCylinderShapeWidget.h>
#include <widget/style/SpinBoxStyleHelper.h>

namespace urchin {

    BodyCylinderShapeWidget::BodyCylinderShapeWidget(const ObjectEntity* objectEntity) :
            BodyShapeWidget(objectEntity) {
        auto* radiusLabel = new QLabel("Radius:");
        mainLayout->addWidget(radiusLabel, 0, 0);

        radius = new QDoubleSpinBox();
        mainLayout->addWidget(radius, 0, 1);
        SpinBoxStyleHelper::applyDefaultStyleOn(radius);
        radius->setMinimum(0.001);
        connect(radius, SIGNAL(valueChanged(double)), this, SLOT(updateBodyShape()));

        auto* heightLabel = new QLabel("Height:");
        mainLayout->addWidget(heightLabel, 1, 0);

        height = new QDoubleSpinBox();
        mainLayout->addWidget(height, 1, 1);
        SpinBoxStyleHelper::applyDefaultStyleOn(height);
        height->setMinimum(0.001);
        connect(height, SIGNAL(valueChanged(double)), this, SLOT(updateBodyShape()));

        auto* orientationLabel = new QLabel("Orientation:");
        mainLayout->addWidget(orientationLabel, 2, 0);

        orientation = new QComboBox();
        mainLayout->addWidget(orientation, 2, 1);
        orientation->addItem("X", QVariant(CylinderShape<float>::CylinderOrientation::CYLINDER_X));
        orientation->addItem("Y", QVariant(CylinderShape<float>::CylinderOrientation::CYLINDER_Y));
        orientation->addItem("Z", QVariant(CylinderShape<float>::CylinderOrientation::CYLINDER_Z));
        connect(orientation, SIGNAL(currentIndexChanged(int)), this, SLOT(updateBodyShape()));
    }

    std::string BodyCylinderShapeWidget::getBodyShapeName() const {
        return CYLINDER_SHAPE_LABEL;
    }

    void BodyCylinderShapeWidget::doSetupShapePropertiesFrom(const CollisionShape3D& shape) {
        const auto& cylinderShape = static_cast<const CollisionCylinderShape&>(shape);

        radius->setValue(cylinderShape.getRadius());
        height->setValue(cylinderShape.getHeight());

        int index = orientation->findData(cylinderShape.getCylinderOrientation());
        if (index != -1) {
            orientation->setCurrentIndex(index);
        }
    }

    std::unique_ptr<const CollisionShape3D> BodyCylinderShapeWidget::createBodyShape() const {
        QVariant variant = orientation->currentData();
        auto orientation = static_cast<CylinderShape<float>::CylinderOrientation>(variant.toInt());

        return std::make_unique<const CollisionCylinderShape>(radius->value(), height->value(), orientation);
    }
}
