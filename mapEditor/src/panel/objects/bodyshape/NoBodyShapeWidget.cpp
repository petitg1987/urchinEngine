#include <QtWidgets/QLabel>

#include "NoBodyShapeWidget.h"

namespace urchin {

    NoBodyShapeWidget::NoBodyShapeWidget(const SceneObject *sceneObject) :
            BodyShapeWidget(sceneObject) {

    }

    std::string NoBodyShapeWidget::getBodyShapeName() const {
        return "/";
    }

    void NoBodyShapeWidget::doSetupShapePropertiesFrom(std::shared_ptr<const CollisionShape3D>) {
        //nothing to do
    }

    std::shared_ptr<const CollisionShape3D> NoBodyShapeWidget::createBodyShape() const {
        //nothing to do
        return std::shared_ptr<const CollisionShape3D>(nullptr);
    }
}
