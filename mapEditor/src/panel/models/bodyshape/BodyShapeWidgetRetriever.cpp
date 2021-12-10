#include <stdexcept>

#include <panel/models/bodyshape/BodyShapeWidgetRetriever.h>
#include <panel/models/bodyshape/BodyShapeWidget.h>
#include <panel/models/bodyshape/BodySphereShapeWidget.h>
#include <panel/models/bodyshape/BodyBoxShapeWidget.h>
#include <panel/models/bodyshape/BodyCapsuleShapeWidget.h>
#include <panel/models/bodyshape/BodyCylinderShapeWidget.h>
#include <panel/models/bodyshape/BodyConeShapeWidget.h>
#include <panel/models/bodyshape/BodyConvexHullShapeWidget.h>
#include <panel/models/bodyshape/BodyCompoundShapeWidget.h>

namespace urchin {

    BodyShapeWidgetRetriever::BodyShapeWidgetRetriever(const ObjectEntity* objectEntity) :
            objectEntity(objectEntity) {

    }

    std::unique_ptr<BodyShapeWidget> BodyShapeWidgetRetriever::createBodyShapeWidget(CollisionShape3D::ShapeType shapeType) {
        std::unique_ptr<BodyShapeWidget> bodyShapeWidget;

        if (shapeType == CollisionShape3D::ShapeType::SPHERE_SHAPE) {
            bodyShapeWidget = std::make_unique<BodySphereShapeWidget>(objectEntity);
        } else if (shapeType == CollisionShape3D::ShapeType::BOX_SHAPE) {
            bodyShapeWidget = std::make_unique<BodyBoxShapeWidget>(objectEntity);
        } else if (shapeType == CollisionShape3D::ShapeType::CAPSULE_SHAPE) {
            bodyShapeWidget = std::make_unique<BodyCapsuleShapeWidget>(objectEntity);
        } else if (shapeType == CollisionShape3D::ShapeType::CYLINDER_SHAPE) {
            bodyShapeWidget = std::make_unique<BodyCylinderShapeWidget>(objectEntity);
        } else if (shapeType == CollisionShape3D::ShapeType::CONE_SHAPE) {
            bodyShapeWidget = std::make_unique<BodyConeShapeWidget>(objectEntity);
        } else if (shapeType == CollisionShape3D::ShapeType::CONVEX_HULL_SHAPE) {
            bodyShapeWidget = std::make_unique<BodyConvexHullShapeWidget>(objectEntity);
        } else if (shapeType == CollisionShape3D::ShapeType::COMPOUND_SHAPE) {
            bodyShapeWidget = std::make_unique<BodyCompoundShapeWidget>(objectEntity);
        } else {
            throw std::invalid_argument("Unknown shape type to retrieve body shape widget: " + std::to_string(shapeType));
        }

        return bodyShapeWidget;
    }

}
