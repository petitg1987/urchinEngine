#include <QtWidgets/QLabel>
#include <QtWidgets/QBoxLayout>

#include "panel/object/sound/soundshape/SoundSphereShapeWidget.h"
#include "widget/style/SpinBoxStyleHelper.h"

namespace urchin {

    SoundSphereShapeWidget::SoundSphereShapeWidget(const ObjectEntity& objectEntity) :
            SoundShapeWidget(objectEntity) {
        auto* positionLabel = new QLabel("Position rel.:");
        mainLayout->addWidget(positionLabel, 1, 0);

        auto* positionLayout = new QHBoxLayout();
        mainLayout->addLayout(positionLayout, 1, 1);
        positionX = new QDoubleSpinBox();
        positionLayout->addWidget(positionX);
        SpinBoxStyleHelper::applyDefaultStyleOn(positionX);
        connect(positionX, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));
        positionY = new QDoubleSpinBox();
        positionLayout->addWidget(positionY);
        SpinBoxStyleHelper::applyDefaultStyleOn(positionY);
        connect(positionY, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));
        positionZ = new QDoubleSpinBox();
        positionLayout->addWidget(positionZ);
        SpinBoxStyleHelper::applyDefaultStyleOn(positionZ);
        connect(positionZ, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));

        auto* radiusLabel = new QLabel("Radius:");
        mainLayout->addWidget(radiusLabel, 2, 0);

        radius = new QDoubleSpinBox();
        mainLayout->addWidget(radius, 2, 1);
        SpinBoxStyleHelper::applyDefaultStyleOn(radius, 0.0);
        connect(radius, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));
    }

    std::string SoundSphereShapeWidget::getSoundShapeName() const {
        return SPHERE_SHAPE_LABEL;
    }

    void SoundSphereShapeWidget::doSetupShapePropertiesFrom(const SoundShape& shape) {
        const auto& sphereShape = static_cast<const SoundSphere&>(shape);

        Point3<float> absoluteShapePosition = sphereShape.getCenterPosition();
        Point3<float> relativeShapePosition = absoluteShapePosition - getObjectEntity().getModel()->getTransform().getPosition();

        positionX->setValue(relativeShapePosition.X);
        positionY->setValue(relativeShapePosition.Y);
        positionZ->setValue(relativeShapePosition.Z);

        radius->setValue(sphereShape.getRadius());
    }

    SoundShape* SoundSphereShapeWidget::createSoundShape() const {
        Point3 relativeShapePosition((float)positionX->value(), (float)positionY->value(), (float)positionZ->value());
        Point3 absoluteShapePosition = getObjectEntity().getModel()->getTransform().getPosition() + relativeShapePosition;

        return new SoundSphere((float)radius->value(), absoluteShapePosition, getMarginValue());
    }
}
