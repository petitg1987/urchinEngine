#include <stdexcept>

#include "CollisionShapeReaderWriterRetriever.h"
#include "resources/object/bodyshape/CollisionSphereReaderWriter.h"
#include "resources/object/bodyshape/CollisionCapsuleReaderWriter.h"
#include "resources/object/bodyshape/CollisionCylinderReaderWriter.h"
#include "resources/object/bodyshape/CollisionConeReaderWriter.h"
#include "resources/object/bodyshape/CollisionBoxReaderWriter.h"
#include "resources/object/bodyshape/CollisionConvexHullReaderWriter.h"
#include "resources/object/bodyshape/CollisionCompoundShapeReaderWriter.h"

namespace urchin {

    std::shared_ptr<CollisionShapeReaderWriter> CollisionShapeReaderWriterRetriever::retrieveShapeReaderWriter(const std::shared_ptr<XmlChunk>& shapeChunk) {
        std::string shapeType = shapeChunk->getAttributeValue(TYPE_ATTR);
        if (shapeType == SPHERE_VALUE) {
            return std::make_shared<CollisionSphereReaderWriter>();
        } else if (shapeType == CAPSULE_VALUE) {
            return std::make_shared<CollisionCapsuleReaderWriter>();
        } else if (shapeType == CYLINDER_VALUE) {
            return std::make_shared<CollisionCylinderReaderWriter>();
        } else if (shapeType == CONE_VALUE) {
            return std::make_shared<CollisionConeReaderWriter>();
        } else if (shapeType == BOX_VALUE) {
            return std::make_shared<CollisionBoxReaderWriter>();
        } else if (shapeType == CONVEX_HULL_VALUE) {
            return std::make_shared<CollisionConvexHullReaderWriter>();
        } else if (shapeType == COMPOUND_SHAPE_VALUE) {
            return std::make_shared<CollisionCompoundShapeReaderWriter>();
        }

        throw std::invalid_argument("Unknown shape type: " + shapeType);
    }

    std::shared_ptr<CollisionShapeReaderWriter> CollisionShapeReaderWriterRetriever::retrieveShapeReaderWriter(const std::shared_ptr<const CollisionShape3D>& collisionShape) {
        CollisionShape3D::ShapeType shapeType = collisionShape->getShapeType();
        if (shapeType == CollisionShape3D::SPHERE_SHAPE) {
            return std::make_shared<CollisionSphereReaderWriter>();
        } else if (shapeType == CollisionShape3D::CAPSULE_SHAPE) {
            return std::make_shared<CollisionCapsuleReaderWriter>();
        } else if (shapeType == CollisionShape3D::CYLINDER_SHAPE) {
            return std::make_shared<CollisionCylinderReaderWriter>();
        } else if (shapeType == CollisionShape3D::CONE_SHAPE) {
            return std::make_shared<CollisionConeReaderWriter>();
        } else if (shapeType == CollisionShape3D::BOX_SHAPE) {
            return std::make_shared<CollisionBoxReaderWriter>();
        } else if (shapeType == CollisionShape3D::CONVEX_HULL_SHAPE) {
            return std::make_shared<CollisionConvexHullReaderWriter>();
        } else if (shapeType == CollisionShape3D::COMPOUND_SHAPE) {
            return std::make_shared<CollisionCompoundShapeReaderWriter>();
        }

        throw std::invalid_argument("Unknown shape type: " + std::to_string(shapeType));
    }

}
