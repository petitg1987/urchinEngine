#include <QtWidgets/QLabel>

#include <panel/objects/sound/soundshape/SoundShapeWidget.h>
#include <widget/style/SpinBoxStyleHelper.h>

namespace urchin {

    SoundShapeWidget::SoundShapeWidget() :
            disableShapeEvent(false),
            shape(nullptr) {
        setContentsMargins(0, 0, 0, 0);

        mainLayout = new QGridLayout(this);
        mainLayout->setAlignment(Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignTop);
        mainLayout->setContentsMargins(0, 0, 0, 0);

        auto* marginLabel = new QLabel("Margin:");
        mainLayout->addWidget(marginLabel, 0, 0);

        margin = new QDoubleSpinBox();
        mainLayout->addWidget(margin, 0, 1);
        SpinBoxStyleHelper::applyDefaultStyleOn(margin);
        margin->setMinimum(0.0);
        connect(margin, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));
    }

    const SoundShape* SoundShapeWidget::retrieveShape() {
        if (!shape) {
            shape = createSoundShape();
        }
        return shape;
    }

    void SoundShapeWidget::setupShapePropertiesFrom(const SoundShape& shape) {
        disableShapeEvent = true;

        margin->setValue(shape.getMargin());

        doSetupShapePropertiesFrom(shape);

        disableShapeEvent = false;
    }

    void SoundShapeWidget::updateSoundShape() {
        if (!disableShapeEvent) {
            shape = createSoundShape();

            emit soundShapeChange(shape);
        }
    }

    float SoundShapeWidget::getMarginValue() const {
        return (float)margin->value();
    }
}
