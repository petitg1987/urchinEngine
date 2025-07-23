#include <stdexcept>

#include "panel/object/physics/bodyshape/BodyShapeWidgetRetriever.h"
#include "panel/object/physics/bodyshape/BodyShapeWidget.h"
#include "panel/object/physics/bodyshape/BodySphereShapeWidget.h"
#include "panel/object/physics/bodyshape/BodyBoxShapeWidget.h"
#include "panel/object/physics/bodyshape/BodyCapsuleShapeWidget.h"
#include "panel/object/physics/bodyshape/BodyCylinderShapeWidget.h"
#include "panel/object/physics/bodyshape/BodyConeShapeWidget.h"
#include "panel/object/physics/bodyshape/BodyConvexHullShapeWidget.h"
#include "panel/object/physics/bodyshape/BodyCompoundShapeWidget.h"

namespace urchin {

    BodyShapeWidgetRetriever::BodyShapeWidgetRetriever(const ObjectEntity* objectEntity) :
            objectEntity(objectEntity) {

    }

    BodyShapeWidget* BodyShapeWidgetRetriever::createBodyShapeWidget(CollisionShape3D::ShapeType shapeType) const {
        if (shapeType == CollisionShape3D::ShapeType::SPHERE_SHAPE) {
            return new BodySphereShapeWidget(objectEntity);
        } else if (shapeType == CollisionShape3D::ShapeType::BOX_SHAPE) {
            return new BodyBoxShapeWidget(objectEntity);
        } else if (shapeType == CollisionShape3D::ShapeType::CAPSULE_SHAPE) {
            return new BodyCapsuleShapeWidget(objectEntity);
        } else if (shapeType == CollisionShape3D::ShapeType::CYLINDER_SHAPE) {
            return new BodyCylinderShapeWidget(objectEntity);
        } else if (shapeType == CollisionShape3D::ShapeType::CONE_SHAPE) {
            return new BodyConeShapeWidget(objectEntity);
        } else if (shapeType == CollisionShape3D::ShapeType::CONVEX_HULL_SHAPE) {
            return new BodyConvexHullShapeWidget(objectEntity);
        } else if (shapeType == CollisionShape3D::ShapeType::COMPOUND_SHAPE) {
            return new BodyCompoundShapeWidget(objectEntity);
        }

        throw std::invalid_argument("Unknown shape type to retrieve body shape widget: " + std::to_string(shapeType));
    }

}
