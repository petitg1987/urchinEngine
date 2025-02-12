#include <QtWidgets/QLabel>
#include <QtWidgets/QBoxLayout>

#include <panel/objects/sound/soundshape/SoundBoxShapeWidget.h>
#include <widget/style/SpinBoxStyleHelper.h>
#include <widget/style/ComboBoxStyleHelper.h>

namespace urchin {

    SoundBoxShapeWidget::SoundBoxShapeWidget(const ObjectEntity& objectEntity) :
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

        auto* halfSizesLabel = new QLabel("Half Sizes:");
        mainLayout->addWidget(halfSizesLabel, 2, 0);

        auto* halfSizeLayout = new QHBoxLayout();
        mainLayout->addLayout(halfSizeLayout, 2, 1);
        halfSizeX = new QDoubleSpinBox();
        halfSizeLayout->addWidget(halfSizeX);
        SpinBoxStyleHelper::applyDefaultStyleOn(halfSizeX);
        connect(halfSizeX, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));
        halfSizeY = new QDoubleSpinBox();
        halfSizeLayout->addWidget(halfSizeY);
        SpinBoxStyleHelper::applyDefaultStyleOn(halfSizeY);
        connect(halfSizeY, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));
        halfSizeZ = new QDoubleSpinBox();
        halfSizeLayout->addWidget(halfSizeZ);
        SpinBoxStyleHelper::applyDefaultStyleOn(halfSizeZ);
        connect(halfSizeZ, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));

        auto* orientationTypeLabel = new QLabel("Orient. Type:");
        mainLayout->addWidget(orientationTypeLabel, 3, 0);

        orientationType = new QComboBox();
        mainLayout->addWidget(orientationType, 3, 1);
        ComboBoxStyleHelper::applyOrientationStyleOn(orientationType);
        connect(orientationType, SIGNAL(currentIndexChanged(int)), this, SLOT(updateShapeOrientationType()));

        auto* eulerAngleLabel = new QLabel("Euler Angle:");
        mainLayout->addWidget(eulerAngleLabel, 4, 0);

        auto* eulerAxisLayout = new QHBoxLayout();
        mainLayout->addLayout(eulerAxisLayout, 4, 1);
        eulerAxis0 = new QDoubleSpinBox();
        eulerAxisLayout->addWidget(eulerAxis0);
        SpinBoxStyleHelper::applyAngleStyleOn(eulerAxis0);
        connect(eulerAxis0, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));
        eulerAxis1 = new QDoubleSpinBox();
        eulerAxisLayout->addWidget(eulerAxis1);
        SpinBoxStyleHelper::applyAngleStyleOn(eulerAxis1);
        connect(eulerAxis1, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));
        eulerAxis2 = new QDoubleSpinBox();
        eulerAxisLayout->addWidget(eulerAxis2);
        SpinBoxStyleHelper::applyAngleStyleOn(eulerAxis2);
        connect(eulerAxis2, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));
    }

    std::string SoundBoxShapeWidget::getSoundShapeName() const {
        return BOX_SHAPE_LABEL;
    }

    void SoundBoxShapeWidget::doSetupShapePropertiesFrom(const SoundShape& shape) {
        const auto& boxShape = static_cast<const SoundBox&>(shape);

        Point3<float> absoluteShapePosition = boxShape.getCenterPosition();
        Point3<float> relativeShapePosition = absoluteShapePosition - getObjectEntity().getModel()->getTransform().getPosition();

        positionX->setValue(relativeShapePosition.X);
        positionY->setValue(relativeShapePosition.Y);
        positionZ->setValue(relativeShapePosition.Z);

        halfSizeX->setValue(boxShape.getHalfSizes().X);
        halfSizeY->setValue(boxShape.getHalfSizes().Y);
        halfSizeZ->setValue(boxShape.getHalfSizes().Z);

        Vector3<float> eulerAngle = boxShape.getOrientation().toEulerAngle(Quaternion<float>::RotationSequence::XYZ);
        this->eulerAxis0->setValue(AngleConverter<double>::toDegree(eulerAngle[0]));
        this->eulerAxis1->setValue(AngleConverter<double>::toDegree(eulerAngle[1]));
        this->eulerAxis2->setValue(AngleConverter<double>::toDegree(eulerAngle[2]));
    }

    void SoundBoxShapeWidget::updateShapeOrientationType() {
        if (!disableShapeEvent) {
            const auto* boxShape = static_cast<const SoundBox*>(retrieveShape());

            QVariant variant = orientationType->currentData();
            auto newRotationSequence = static_cast<Quaternion<float>::RotationSequence>(variant.toInt());
            Vector3<float> eulerAngle = boxShape->getOrientation().toEulerAngle(newRotationSequence);

            eulerAxis0->setValue(AngleConverter<float>::toDegree(eulerAngle.X));
            eulerAxis1->setValue(AngleConverter<float>::toDegree(eulerAngle.Y));
            eulerAxis2->setValue(AngleConverter<float>::toDegree(eulerAngle.Z));

            updateSoundShape();
        }
    }

    SoundShape* SoundBoxShapeWidget::createSoundShape() const {
        Point3 relativeShapePosition((float)positionX->value(), (float)positionY->value(), (float)positionZ->value());
        Point3 absoluteShapePosition = getObjectEntity().getModel()->getTransform().getPosition() + relativeShapePosition;

        Vector3 halfSizes((float)halfSizeX->value(), (float)halfSizeY->value(), (float)halfSizeZ->value());

        Vector3 eulerAngle(
            AngleConverter<float>::toRadian((float)eulerAxis0->value()),
            AngleConverter<float>::toRadian((float)eulerAxis1->value()),
            AngleConverter<float>::toRadian((float)eulerAxis2->value())
        );
        QVariant variant = orientationType->currentData();
        auto rotationSequence = static_cast<Quaternion<float>::RotationSequence>(variant.toInt());
        Quaternion<float> orientation = Quaternion<float>::fromEuler(eulerAngle, rotationSequence);

        return new SoundBox(halfSizes, absoluteShapePosition, orientation, getMarginValue());
    }
}
