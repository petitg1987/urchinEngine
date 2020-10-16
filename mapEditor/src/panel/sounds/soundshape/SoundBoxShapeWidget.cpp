#include <QtWidgets/QLabel>
#include <QtWidgets/QHBoxLayout>

#include "SoundBoxShapeWidget.h"
#include "widget/style/SpinBoxStyleHelper.h"
#include "widget/style/ComboBoxStyleHelper.h"

namespace urchin {

    SoundBoxShapeWidget::SoundBoxShapeWidget(const SceneSound *sceneSound) :
            SoundShapeWidget(sceneSound) {
        auto *positionLabel = new QLabel("Position:");
        mainLayout->addWidget(positionLabel, 1, 0);

        auto *positionLayout = new QHBoxLayout();
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

        auto *halfSizesLabel = new QLabel("Half Sizes:");
        mainLayout->addWidget(halfSizesLabel, 2, 0);

        auto *halfSizeLayout = new QHBoxLayout();
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

        auto *orientationTypeLabel = new QLabel("Orient. Type:");
        mainLayout->addWidget(orientationTypeLabel, 3, 0);

        orientationType = new QComboBox();
        mainLayout->addWidget(orientationType, 3, 1);
        ComboBoxStyleHelper::applyOrientationStyleOn(orientationType);
        connect(orientationType, SIGNAL(currentIndexChanged(int)), this, SLOT(updateShapeOrientationType()));

        auto *eulerAngleLabel = new QLabel("Euler Angle:");
        mainLayout->addWidget(eulerAngleLabel, 4, 0);

        auto *eulerAxisLayout = new QHBoxLayout();
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

    void SoundBoxShapeWidget::doSetupShapePropertiesFrom(const SoundShape *shape) {
        const auto *boxShape = dynamic_cast<const SoundBox *>(shape);

        positionX->setValue(boxShape->getCenterPosition().X);
        positionY->setValue(boxShape->getCenterPosition().Y);
        positionZ->setValue(boxShape->getCenterPosition().Z);

        halfSizeX->setValue(boxShape->getHalfSizes().X);
        halfSizeY->setValue(boxShape->getHalfSizes().Y);
        halfSizeZ->setValue(boxShape->getHalfSizes().Z);

        Vector3<float> eulerAngle = boxShape->getOrientation().toEulerAngle(Quaternion<float>::RotationSequence::XYZ);
        this->eulerAxis0->setValue(AngleConverter<double>::toDegree(eulerAngle[0]));
        this->eulerAxis1->setValue(AngleConverter<double>::toDegree(eulerAngle[1]));
        this->eulerAxis2->setValue(AngleConverter<double>::toDegree(eulerAngle[2]));
    }

    void SoundBoxShapeWidget::updateShapeOrientationType() {
        if(!disableShapeEvent) {
            const auto *boxShape = dynamic_cast<const SoundBox *>(retrieveShape());

            QVariant variant = orientationType->currentData();
            auto newRotationSequence = static_cast<Quaternion<float>::RotationSequence>(variant.toInt());
            Vector3<float> eulerAngle = boxShape->getOrientation().toEulerAngle(newRotationSequence);

            eulerAxis0->setValue(AngleConverter<float>::toDegree(eulerAngle.X));
            eulerAxis1->setValue(AngleConverter<float>::toDegree(eulerAngle.Y));
            eulerAxis2->setValue(AngleConverter<float>::toDegree(eulerAngle.Z));

            updateSoundShape();
        }
    }

    const SoundShape *SoundBoxShapeWidget::createSoundShape() const {
        Point3<float> position(positionX->value(), positionY->value(), positionZ->value());

        Vector3<float> halfSizes(halfSizeX->value(), halfSizeY->value(), halfSizeZ->value());

        Vector3<float> eulerAngle(
            AngleConverter<float>::toRadian(eulerAxis0->value()),
            AngleConverter<float>::toRadian(eulerAxis1->value()),
            AngleConverter<float>::toRadian(eulerAxis2->value())
        );
        QVariant variant = orientationType->currentData();
        auto rotationSequence = static_cast<Quaternion<float>::RotationSequence>(variant.toInt());
        Quaternion<float> orientation(eulerAngle, rotationSequence);

        return new SoundBox(halfSizes, position, orientation, getMarginValue());
    }
}
