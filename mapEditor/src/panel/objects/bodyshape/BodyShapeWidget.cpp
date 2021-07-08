#include <stdexcept>
#include <utility>

#include <panel/objects/bodyshape/BodyShapeWidget.h>

namespace urchin {

    BodyShapeWidget::BodyShapeWidget(const SceneObject* sceneObject) :
            disableShapeEvent(false),
            sceneObject(sceneObject) {
        setContentsMargins(0, 0, 0, 0);

        mainLayout = new QGridLayout(this);
        mainLayout->setAlignment(Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignTop);
        mainLayout->setContentsMargins(0, 0, 0, 0);
    }

    const SceneObject* BodyShapeWidget::getSceneObject() const {
        return sceneObject;
    }

    std::unique_ptr<const CollisionShape3D> BodyShapeWidget::moveShape() {
        if (!shape) {
            shape = createBodyShape();
        }
        return std::move(shape);
    }

    void BodyShapeWidget::setupShapePropertiesFrom(const CollisionShape3D& shape) {
        disableShapeEvent = true;
        doSetupShapePropertiesFrom(shape);
        disableShapeEvent = false;
    }

    void BodyShapeWidget::updateBodyShape() {
        if (!disableShapeEvent) {
            shape = createBodyShape();
            emit bodyShapeChange(shape);
        }
    }
}
