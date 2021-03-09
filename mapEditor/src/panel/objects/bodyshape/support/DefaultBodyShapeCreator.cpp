#include "DefaultBodyShapeCreator.h"

namespace urchin {

    DefaultBodyShapeCreator::DefaultBodyShapeCreator(const SceneObject* sceneObject) :
        sceneObject(sceneObject) {

    }

    std::shared_ptr<const CollisionShape3D> DefaultBodyShapeCreator::createDefaultBodyShape(CollisionShape3D::ShapeType shapeType) const {
        CollisionShape3D *shape;
        const AABBox<float>& modelAABBox = sceneObject->getModel()->getLocalAABBox();

        if (shapeType == CollisionShape3D::ShapeType::BOX_SHAPE) {
            shape = new CollisionBoxShape(modelAABBox.getHalfSizes());
        } else if (shapeType == CollisionShape3D::ShapeType::SPHERE_SHAPE) {
            shape = new CollisionSphereShape(modelAABBox.getMaxHalfSize());
        } else if (shapeType == CollisionShape3D::ShapeType::CAPSULE_SHAPE) {
            float radius = std::max(modelAABBox.getHalfSizes()[1],  modelAABBox.getHalfSizes()[2]);
            float cylinderHeight = modelAABBox.getHalfSizes()[0]*2.0f;

            shape = new CollisionCapsuleShape(radius, cylinderHeight, CapsuleShape<float>::CAPSULE_X);
        } else if (shapeType == CollisionShape3D::ShapeType::CYLINDER_SHAPE) {
            float radius = std::max(modelAABBox.getHalfSizes()[1],  modelAABBox.getHalfSizes()[2]);
            float height = modelAABBox.getHalfSizes()[0]*2.0f;

            shape = new CollisionCylinderShape(radius, height, CylinderShape<float>::CYLINDER_X);
        } else if (shapeType == CollisionShape3D::ShapeType::CONE_SHAPE) {
            float radius = std::max(modelAABBox.getHalfSizes()[1],  modelAABBox.getHalfSizes()[2]);
            float height = modelAABBox.getHalfSizes()[0]*2.0f;

            shape = new CollisionConeShape(radius, height, ConeShape<float>::CONE_X_POSITIVE);
        } else if (shapeType == CollisionShape3D::ShapeType::CONVEX_HULL_SHAPE) {
            ConvexHullShape3D<float>* convexHullShape = buildConvexHullShape(sceneObject->getModel());
            shape = new CollisionConvexHullShape(convexHullShape);
        } else if (shapeType == CollisionShape3D::ShapeType::COMPOUND_SHAPE) {
            std::vector<std::shared_ptr<const LocalizedCollisionShape>> localizedCollisionShapes;

            std::shared_ptr<LocalizedCollisionShape> boxLocalizedShape = std::make_shared<LocalizedCollisionShape>();
            boxLocalizedShape->position = 0;
            boxLocalizedShape->shape = std::make_shared<const CollisionBoxShape>(modelAABBox.getHalfSizes());
            boxLocalizedShape->transform = PhysicsTransform();
            localizedCollisionShapes.push_back(boxLocalizedShape);

            shape = new CollisionCompoundShape(localizedCollisionShapes);
        } else {
            throw std::invalid_argument("Unknown shape type to create default body shape: " + std::to_string(shapeType));
        }

        float scale = sceneObject->getModel()->getTransform().getScale();
        std::shared_ptr<const CollisionShape3D> scaledShape = shape->scale(scale);
        delete shape;

        return scaledShape;
    }

    ConvexHullShape3D<float>* DefaultBodyShapeCreator::buildConvexHullShape(const Model* model) const {
        std::set<Point3<float>> allVertices;
        for (const auto* constMesh : model->getConstMeshes()->getConstMeshes()) {
            for (unsigned int i = 0; i < constMesh->getNumberVertices(); i++) {
                allVertices.insert(constMesh->getBaseVertices()[i]);
            }
        }

        return new ConvexHullShape3D<float>(std::vector<Point3<float>>(allVertices.begin(), allVertices.end()));
    }
}
