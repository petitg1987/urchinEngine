#include <stdexcept>
#include <utility>

#include <panel/models/bodyshape/BodyShapeWidget.h>

namespace urchin {

    BodyShapeWidget::BodyShapeWidget(const SceneModel* sceneModel) :
            disableShapeEvent(false),
            sceneModel(sceneModel) {
        setContentsMargins(0, 0, 0, 0);

        mainLayout = new QGridLayout(this);
        mainLayout->setAlignment(Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignTop);
        mainLayout->setContentsMargins(0, 0, 0, 0);
    }

    const SceneModel* BodyShapeWidget::getSceneModel() const {
        return sceneModel;
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
