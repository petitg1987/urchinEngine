#include <stdexcept>

#include "BodyShapeWidgetRetriever.h"
#include "panel/objects/bodyshape/BodyShapeWidget.h"
#include "panel/objects/bodyshape/NoBodyShapeWidget.h"
#include "panel/objects/bodyshape/BodySphereShapeWidget.h"
#include "panel/objects/bodyshape/BodyBoxShapeWidget.h"
#include "panel/objects/bodyshape/BodyCapsuleShapeWidget.h"
#include "panel/objects/bodyshape/BodyCylinderShapeWidget.h"
#include "panel/objects/bodyshape/BodyConeShapeWidget.h"
#include "panel/objects/bodyshape/BodyConvexHullShapeWidget.h"
#include "panel/objects/bodyshape/BodyCompoundShapeWidget.h"

namespace urchin {

    BodyShapeWidgetRetriever::BodyShapeWidgetRetriever(const SceneObject* sceneObject) :
            sceneObject(sceneObject) {

    }

    BodyShapeWidget* BodyShapeWidgetRetriever::createBodyShapeWidget(const std::shared_ptr<const CollisionShape3D>& shape) {
        if (shape) {
            return createBodyShapeWidget(shape->getShapeType());
        }

        return new NoBodyShapeWidget(sceneObject);
    }

    BodyShapeWidget* BodyShapeWidgetRetriever::createBodyShapeWidget(CollisionShape3D::ShapeType shapeType) {
        BodyShapeWidget *bodyShapeWidget;

        if (shapeType==CollisionShape3D::ShapeType::SPHERE_SHAPE) {
            bodyShapeWidget = new BodySphereShapeWidget(sceneObject);
        } else if (shapeType==CollisionShape3D::ShapeType::BOX_SHAPE) {
            bodyShapeWidget = new BodyBoxShapeWidget(sceneObject);
        } else if (shapeType==CollisionShape3D::ShapeType::CAPSULE_SHAPE) {
            bodyShapeWidget = new BodyCapsuleShapeWidget(sceneObject);
        } else if (shapeType==CollisionShape3D::ShapeType::CYLINDER_SHAPE) {
            bodyShapeWidget = new BodyCylinderShapeWidget(sceneObject);
        } else if (shapeType==CollisionShape3D::ShapeType::CONE_SHAPE) {
            bodyShapeWidget = new BodyConeShapeWidget(sceneObject);
        } else if (shapeType==CollisionShape3D::ShapeType::CONVEX_HULL_SHAPE) {
            bodyShapeWidget = new BodyConvexHullShapeWidget(sceneObject);
        } else if (shapeType==CollisionShape3D::ShapeType::COMPOUND_SHAPE) {
            bodyShapeWidget = new BodyCompoundShapeWidget(sceneObject);
        } else {
            throw std::invalid_argument("Unknown shape type to retrieve body shape widget: " + std::to_string(shapeType));
        }

        return bodyShapeWidget;
    }

}
