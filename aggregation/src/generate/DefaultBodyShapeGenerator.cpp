#include <generate/DefaultBodyShapeGenerator.h>

namespace urchin {

    DefaultBodyShapeGenerator::DefaultBodyShapeGenerator(const ObjectEntity& objectEntity) :
            objectEntity(objectEntity) {

    }

    std::unique_ptr<const CollisionShape3D> DefaultBodyShapeGenerator::generate(CollisionShape3D::ShapeType shapeType) const {
        std::map<std::string, std::string, std::less<>> params;
        return generate(shapeType, params);
    }

    std::unique_ptr<const CollisionShape3D> DefaultBodyShapeGenerator::generate(CollisionShape3D::ShapeType shapeType,
            const std::map<std::string, std::string, std::less<>>& params) const {
        std::unique_ptr<CollisionShape3D> shape;
        const AABBox<float>& modelAABBox = objectEntity.getModel()->getLocalAABBox();

        if (shapeType == CollisionShape3D::ShapeType::BOX_SHAPE) {
            shape = std::make_unique<CollisionBoxShape>(modelAABBox.getHalfSizes());
        } else if (shapeType == CollisionShape3D::ShapeType::SPHERE_SHAPE) {
            shape = std::make_unique<CollisionSphereShape>(modelAABBox.getMaxHalfSize());
        } else if (shapeType == CollisionShape3D::ShapeType::CAPSULE_SHAPE) {
            std::size_t axis = params.contains("axis") ? TypeConverter::toSize(params.at("axis")) : 0;
            auto orientation = static_cast<CapsuleShape<float>::CapsuleOrientation>(axis);

            float radius = std::max(modelAABBox.getHalfSizes()[(axis + 1) % 3], modelAABBox.getHalfSizes()[(axis + 2) % 3]);
            float cylinderHeight = modelAABBox.getHalfSizes()[axis] * 2.0f; //TODO wrong !

            shape = std::make_unique<CollisionCapsuleShape>(radius, cylinderHeight, orientation);
        } else if (shapeType == CollisionShape3D::ShapeType::CYLINDER_SHAPE) {
            std::size_t axis = params.contains("axis") ? TypeConverter::toSize(params.at("axis")) : 0;
            auto orientation = static_cast<CylinderShape<float>::CylinderOrientation>(axis);

            float radius = std::max(modelAABBox.getHalfSizes()[(axis + 1) % 3], modelAABBox.getHalfSizes()[(axis + 2) % 3]);
            float height = modelAABBox.getHalfSizes()[axis] * 2.0f;

            shape = std::make_unique<CollisionCylinderShape>(radius, height, orientation);
        } else if (shapeType == CollisionShape3D::ShapeType::CONE_SHAPE) {
            float radius = std::max(modelAABBox.getHalfSizes()[1], modelAABBox.getHalfSizes()[2]);
            float height = modelAABBox.getHalfSizes()[0] * 2.0f;

            shape = std::make_unique<CollisionConeShape>(radius, height, ConeShape<float>::CONE_X_POSITIVE);
        } else if (shapeType == CollisionShape3D::ShapeType::CONVEX_HULL_SHAPE) {
            shape = std::make_unique<CollisionConvexHullShape>(buildConvexHullShape());
        } else if (shapeType == CollisionShape3D::ShapeType::COMPOUND_SHAPE) {
            shape = std::make_unique<CollisionCompoundShape>(buildLocalizedCollisionShapes());
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

    std::vector<std::shared_ptr<const LocalizedCollisionShape>> DefaultBodyShapeGenerator::buildLocalizedCollisionShapes() const {
        std::vector<std::shared_ptr<const LocalizedCollisionShape>> result;

        if (objectEntity.getModel()->getConstMeshes()) {
            const std::vector<std::unique_ptr<const ConstMesh>>& constMeshes = objectEntity.getModel()->getConstMeshes()->getConstMeshes();
            result.reserve(constMeshes.size());

            for (const std::unique_ptr<const ConstMesh>& constMesh : constMeshes) {
                std::set<Point3<float>> meshUniqueVertices;
                for (unsigned int i = 0; i < constMesh->getNumberVertices(); i++) {
                    meshUniqueVertices.insert(constMesh->getBaseVertices()[i]);
                }

                try {
                    std::vector<std::unique_ptr<LocalizedCollisionShape>> localizedCollisionShapes = buildBestCollisionShapes(std::vector(meshUniqueVertices.begin(), meshUniqueVertices.end()));
                    for (std::unique_ptr<LocalizedCollisionShape>& localizedCollisionShape : localizedCollisionShapes) {
                        result.push_back(std::move(localizedCollisionShape));
                    }
                } catch (const std::invalid_argument&) {
                    //ignore build convex hull errors
                }
            }
        }

        if (result.empty()) {
            auto boxLocalizedShape = std::make_unique<LocalizedCollisionShape>();
            boxLocalizedShape->shapeIndex = 0;
            boxLocalizedShape->shape = std::make_unique<const CollisionBoxShape>(objectEntity.getModel()->getLocalAABBox().getHalfSizes());
            boxLocalizedShape->transform = PhysicsTransform();
            result.push_back(std::move(boxLocalizedShape));
        }

        return result;
    }

    std::vector<std::unique_ptr<LocalizedCollisionShape>> DefaultBodyShapeGenerator::buildBestCollisionShapes(const std::vector<Point3<float>>& uniqueVertices) const {
        std::vector<ShapeDetectService::LocalizedShape> bestLocalizedShapes = ShapeDetectService().detect(uniqueVertices);

        std::vector<std::unique_ptr<LocalizedCollisionShape>> result;
        result.reserve(bestLocalizedShapes.size());

        std::size_t shapeIndex = 0;
        for (ShapeDetectService::LocalizedShape& localizedShape : bestLocalizedShapes) {
            auto localizedCollisionShape = std::make_unique<LocalizedCollisionShape>();
            localizedCollisionShape->shapeIndex = shapeIndex;
            if (dynamic_cast<ConvexHullShape3D<float>*>(localizedShape.shape.get())) {
                std::unique_ptr<ConvexHullShape3D<float>> convexHull(dynamic_cast<ConvexHullShape3D<float>*>(localizedShape.shape.release()));
                localizedCollisionShape->shape = std::make_unique<const CollisionConvexHullShape>(std::move(convexHull));
            } else if (dynamic_cast<BoxShape<float>*>(localizedShape.shape.get())) {
                std::unique_ptr<BoxShape<float>> boxShape(dynamic_cast<BoxShape<float>*>(localizedShape.shape.release()));
                localizedCollisionShape->shape = std::make_unique<const CollisionBoxShape>(boxShape->getHalfSizes());
            } else if (dynamic_cast<SphereShape<float>*>(localizedShape.shape.get())) {
                std::unique_ptr<SphereShape<float>> sphereShape(dynamic_cast<SphereShape<float>*>(localizedShape.shape.release()));
                localizedCollisionShape->shape = std::make_unique<const CollisionSphereShape>(sphereShape->getRadius());
            } else {
                throw std::runtime_error("Unknown shape type to build optimized collision shape");
            }
            localizedCollisionShape->transform = PhysicsTransform(localizedShape.position, localizedShape.orientation);
            result.push_back(std::move(localizedCollisionShape));

            shapeIndex++;
        }

        return result;
    }

}