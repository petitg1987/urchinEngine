#include <stdexcept>

#include <resources/object/bodyshape/CollisionShapeReaderWriterRetriever.h>
#include <resources/object/bodyshape/CollisionSphereReaderWriter.h>
#include <resources/object/bodyshape/CollisionCapsuleReaderWriter.h>
#include <resources/object/bodyshape/CollisionCylinderReaderWriter.h>
#include <resources/object/bodyshape/CollisionConeReaderWriter.h>
#include <resources/object/bodyshape/CollisionBoxReaderWriter.h>
#include <resources/object/bodyshape/CollisionConvexHullReaderWriter.h>
#include <resources/object/bodyshape/CollisionCompoundShapeReaderWriter.h>

namespace urchin {

    std::unique_ptr<CollisionShapeReaderWriter> CollisionShapeReaderWriterRetriever::retrieveShapeReaderWriter(const UdaChunk* shapeChunk) {
        std::string shapeType = shapeChunk->getAttributeValue(CollisionShapeReaderWriter::TYPE_ATTR);
        if (shapeType == CollisionShapeReaderWriter::SPHERE_VALUE) {
            return std::make_unique<CollisionSphereReaderWriter>();
        } else if (shapeType == CollisionShapeReaderWriter::CAPSULE_VALUE) {
            return std::make_unique<CollisionCapsuleReaderWriter>();
        } else if (shapeType == CollisionShapeReaderWriter::CYLINDER_VALUE) {
            return std::make_unique<CollisionCylinderReaderWriter>();
        } else if (shapeType == CollisionShapeReaderWriter::CONE_VALUE) {
            return std::make_unique<CollisionConeReaderWriter>();
        } else if (shapeType == CollisionShapeReaderWriter::BOX_VALUE) {
            return std::make_unique<CollisionBoxReaderWriter>();
        } else if (shapeType == CollisionShapeReaderWriter::CONVEX_HULL_VALUE) {
            return std::make_unique<CollisionConvexHullReaderWriter>();
        } else if (shapeType == CollisionShapeReaderWriter::COMPOUND_SHAPE_VALUE) {
            return std::make_unique<CollisionCompoundShapeReaderWriter>();
        }

        throw std::invalid_argument("Unknown shape type: " + shapeType);
    }

    std::unique_ptr<CollisionShapeReaderWriter> CollisionShapeReaderWriterRetriever::retrieveShapeReaderWriter(const CollisionShape3D& collisionShape) {
        CollisionShape3D::ShapeType shapeType = collisionShape.getShapeType();
        if (shapeType == CollisionShape3D::SPHERE_SHAPE) {
            return std::make_unique<CollisionSphereReaderWriter>();
        } else if (shapeType == CollisionShape3D::CAPSULE_SHAPE) {
            return std::make_unique<CollisionCapsuleReaderWriter>();
        } else if (shapeType == CollisionShape3D::CYLINDER_SHAPE) {
            return std::make_unique<CollisionCylinderReaderWriter>();
        } else if (shapeType == CollisionShape3D::CONE_SHAPE) {
            return std::make_unique<CollisionConeReaderWriter>();
        } else if (shapeType == CollisionShape3D::BOX_SHAPE) {
            return std::make_unique<CollisionBoxReaderWriter>();
        } else if (shapeType == CollisionShape3D::CONVEX_HULL_SHAPE) {
            return std::make_unique<CollisionConvexHullReaderWriter>();
        } else if (shapeType == CollisionShape3D::COMPOUND_SHAPE) {
            return std::make_unique<CollisionCompoundShapeReaderWriter>();
        }

        throw std::invalid_argument("Unknown shape type: " + std::to_string(shapeType));
    }

}
