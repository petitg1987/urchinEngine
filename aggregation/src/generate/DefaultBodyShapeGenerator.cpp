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
            shape = std::make_unique<CollisionConvexHullShape>(buildConvexHullShape());
        } else if (shapeType == CollisionShape3D::ShapeType::COMPOUND_SHAPE) {
            shape = std::make_unique<CollisionCompoundShape>(buildLocalizedCollisionShape());
        } else {
            throw std::invalid_argument("Unknown shape type to create default body shape: " + std::to_string(shapeType));
        }

        Vector3<float> scale = objectEntity.getModel()->getTransform().getScale();
        return shape->scale(scale);
    }

    std::unique_ptr<ConvexHullShape3D<float>> DefaultBodyShapeGenerator::buildConvexHullShape() const {
        std::set<Point3<float>> uniqueVertices;

        if (objectEntity.getModel()->getConstMeshes()) {
            const std::vector<std::unique_ptr<const ConstMesh>>& constMeshes = objectEntity.getModel()->getConstMeshes()->getConstMeshes();
            for (const std::unique_ptr<const ConstMesh>& constMesh : constMeshes) {
                for (unsigned int i = 0; i < constMesh->getNumberVertices(); i++) {
                    uniqueVertices.insert(constMesh->getBaseVertices()[i]);
                }
            }

            try {
                return std::make_unique<ConvexHullShape3D<float>>(std::vector(uniqueVertices.begin(), uniqueVertices.end()));
            } catch (const std::invalid_argument&) {
                //ignore build convex hull errors
            }
            uniqueVertices.clear();
        }

        for (const Point3<float>& point : objectEntity.getModel()->getLocalAABBox().getPoints()) {
            uniqueVertices.insert(point);
        }
        return std::make_unique<ConvexHullShape3D<float>>(std::vector(uniqueVertices.begin(), uniqueVertices.end()));
    }

    std::vector<std::shared_ptr<const LocalizedCollisionShape>> DefaultBodyShapeGenerator::buildLocalizedCollisionShape() const {
        std::vector<std::shared_ptr<const LocalizedCollisionShape>> localizedCollisionShapes;

        if (objectEntity.getModel()->getConstMeshes()) {
            const std::vector<std::unique_ptr<const ConstMesh>>& constMeshes = objectEntity.getModel()->getConstMeshes()->getConstMeshes();
            localizedCollisionShapes.reserve(constMeshes.size());

            std::size_t shapeIndex = 0;
            for (const std::unique_ptr<const ConstMesh>& constMesh : constMeshes) {
                std::set<Point3<float>> meshUniqueVertices;
                for (unsigned int i = 0; i < constMesh->getNumberVertices(); i++) {
                    meshUniqueVertices.insert(constMesh->getBaseVertices()[i]);
                }

                try {
                    auto localizedShape = std::make_unique<LocalizedCollisionShape>();
                    localizedShape->shapeIndex = shapeIndex++;
                    localizedShape->shape = buildOptimizedCollisionShape(std::vector(meshUniqueVertices.begin(), meshUniqueVertices.end()));
                    localizedShape->transform = PhysicsTransform();
                    localizedCollisionShapes.push_back(std::move(localizedShape));
                } catch (const std::invalid_argument&) {
                    //ignore build convex hull errors
                }
            }
        }

        if (localizedCollisionShapes.empty()) {
            auto boxLocalizedShape = std::make_unique<LocalizedCollisionShape>();
            boxLocalizedShape->shapeIndex = 0;
            boxLocalizedShape->shape = std::make_unique<const CollisionBoxShape>(objectEntity.getModel()->getLocalAABBox().getHalfSizes());
            boxLocalizedShape->transform = PhysicsTransform();
            localizedCollisionShapes.push_back(std::move(boxLocalizedShape));
        }

        return localizedCollisionShapes;
    }

    std::unique_ptr<const CollisionShape3D> DefaultBodyShapeGenerator::buildOptimizedCollisionShape(const std::vector<Point3<float>>& uniqueVertices) const {
        auto convexHullShape = std::make_unique<ConvexHullShape3D<float>>(uniqueVertices);

        if (convexHullShape->getConvexHullPoints().size() == 8) {
            //TODO return box
        }

        return std::make_unique<const CollisionConvexHullShape>(std::move(convexHullShape));
    }

}