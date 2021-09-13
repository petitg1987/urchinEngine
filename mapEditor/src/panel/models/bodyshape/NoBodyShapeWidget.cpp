#include <QtWidgets/QLabel>

#include <panel/models/bodyshape/NoBodyShapeWidget.h>

namespace urchin {

    NoBodyShapeWidget::NoBodyShapeWidget(const SceneModel* sceneModel) :
            BodyShapeWidget(sceneModel) {

    }

    std::string NoBodyShapeWidget::getBodyShapeName() const {
        return "/";
    }

    void NoBodyShapeWidget::doSetupShapePropertiesFrom(const CollisionShape3D&) {
        //nothing to do
    }

    std::unique_ptr<const CollisionShape3D> NoBodyShapeWidget::createBodyShape() const {
        //nothing to do
        return std::unique_ptr<const CollisionShape3D>(nullptr);
    }
}
