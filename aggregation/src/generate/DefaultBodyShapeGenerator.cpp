#include <generate/DefaultBodyShapeGenerator.h>

namespace urchin {

    DefaultBodyShapeGenerator::DefaultBodyShapeGenerator(const ObjectEntity& objectEntity) :
            objectEntity(objectEntity) {

    }

    std::unique_ptr<const CollisionShape3D> DefaultBodyShapeGenerator::generate(CollisionShape3D::ShapeType shapeType) const {
        std::unique_ptr<CollisionShape3D> shape;
        const AABBox<float>& modelAABBox = objectEntity.getModel()->getLocalAABBox();

        if (shapeType == CollisionShape3D::ShapeType::BOX_SHAPE) {
            shape = std::make_unique<CollisionBoxShape>(modelAABBox.getHalfSizes());
        } else if (shapeType == CollisionShape3D::ShapeType::SPHERE_SHAPE) {
            shape = std::make_unique<CollisionSphereShape>(modelAABBox.getMaxHalfSize());
        } else if (shapeType == CollisionShape3D::ShapeType::CAPSULE_SHAPE) {
            float radius = std::max(modelAABBox.getHalfSizes()[1], modelAABBox.getHalfSizes()[2]);
            float cylinderHeight = modelAABBox.getHalfSizes()[0] * 2.0f;

            shape = std::make_unique<CollisionCapsuleShape>(radius, cylinderHeight, CapsuleShape<float>::CAPSULE_X);
        } else if (shapeType == CollisionShape3D::ShapeType::CYLINDER_SHAPE) {
            float radius = std::max(modelAABBox.getHalfSizes()[1], modelAABBox.getHalfSizes()[2]);
            float height = modelAABBox.getHalfSizes()[0] * 2.0f;

            shape = std::make_unique<CollisionCylinderShape>(radius, height, CylinderShape<float>::CYLINDER_X);
        } else if (shapeType == CollisionShape3D::ShapeType::CONE_SHAPE) {
            float radius = std::max(modelAABBox.getHalfSizes()[1], modelAABBox.getHalfSizes()[2]);
            float height = modelAABBox.getHalfSizes()[0] * 2.0f;

            shape = std::make_unique<CollisionConeShape>(radius, height, ConeShape<float>::CONE_X_POSITIVE);
        } else if (shapeType == CollisionShape3D::ShapeType::CONVEX_HULL_SHAPE) {
            auto convexHullShape = buildConvexHullShape(objectEntity.getModel());
            shape = std::make_unique<CollisionConvexHullShape>(std::move(convexHullShape));
        } else if (shapeType == CollisionShape3D::ShapeType::COMPOUND_SHAPE) {
            std::vector<std::shared_ptr<const LocalizedCollisionShape>> localizedCollisionShapes;

            auto boxLocalizedShape = std::make_shared<LocalizedCollisionShape>();
            boxLocalizedShape->position = 0;
            boxLocalizedShape->shape = std::make_unique<const CollisionBoxShape>(modelAABBox.getHalfSizes());
            boxLocalizedShape->transform = PhysicsTransform();
            localizedCollisionShapes.push_back(std::move(boxLocalizedShape));

            shape = std::make_unique<CollisionCompoundShape>(std::move(localizedCollisionShapes));
        } else {
            throw std::invalid_argument("Unknown shape type to create default body shape: " + std::to_string(shapeType));
        }

        Vector3<float> scale = objectEntity.getModel()->getTransform().getScale();
        return shape->scale(scale);
    }

    std::unique_ptr<ConvexHullShape3D<float>> DefaultBodyShapeGenerator::buildConvexHullShape(const Model* model) const {
        std::set<Point3<float>> allVertices;
        for (const auto& constMesh : model->getConstMeshes()->getConstMeshes()) {
            for (unsigned int i = 0; i < constMesh->getNumberVertices(); i++) {
                allVertices.insert(constMesh->getBaseVertices()[i]);
            }
        }

        return std::make_unique<ConvexHullShape3D<float>>(std::vector<Point3<float>>(allVertices.begin(), allVertices.end()));
    }

}