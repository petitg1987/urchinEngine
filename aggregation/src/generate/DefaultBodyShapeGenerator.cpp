#include <generate/DefaultBodyShapeGenerator.h>

namespace urchin {

    DefaultBodyShapeGenerator::DefaultBodyShapeGenerator(const ObjectEntity& objectEntity, const DefaultShapeQuality& defaultShapeQuality) :
            objectEntity(objectEntity) {

        MeshSimplificationService::Config meshSimplificationConfig = {};
        meshSimplificationService = std::make_unique<MeshSimplificationService>(meshSimplificationConfig);

        ShapeDetectService::Config shapeDetectConfig = {
            .voxelizationSize = defaultShapeQuality.getVoxelizationSize(),
            .convexHullMaxPoints = defaultShapeQuality.getConvexHullMaxPoints()
        };
        shapeDetectService = std::make_unique<ShapeDetectService>(shapeDetectConfig);
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
            float cylinderHeight = std::max((modelAABBox.getHalfSizes()[axis] * 2.0f) - 2.0f * radius, 0.01f);

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
        if (objectEntity.getModel()->getConstMeshes()) {
            const Meshes* meshes = objectEntity.getModel()->getMeshes();

            MeshData meshData({}, {});
            for (unsigned int meshIndex = 0; meshIndex < meshes->getNumMeshes(); ++meshIndex) {
                const std::vector<Point3<float>>& vertices = meshes->getMesh(meshIndex).getVertices();
                const std::vector<std::array<uint32_t, 3>>& triangleIndices = meshes->getConstMeshes().getConstMeshes()[meshIndex]->getTrianglesIndices();
                meshData.addNewMesh(vertices, triangleIndices);
            }

            MeshData simplifiedMesh = meshSimplificationService->simplify(meshData);

            try {
                return std::make_unique<ConvexHullShape3D<float>>(simplifiedMesh.getVertices());
            } catch (const std::invalid_argument&) {
                //ignore build convex hull errors
            }
        }

        std::set<Point3<float>> boundingBoxPoints;
        for (const Point3<float>& point : objectEntity.getModel()->getLocalAABBox().getPoints()) {
            boundingBoxPoints.insert(point);
        }
        return std::make_unique<ConvexHullShape3D<float>>(std::vector(boundingBoxPoints.begin(), boundingBoxPoints.end()));
    }

    std::vector<std::shared_ptr<const LocalizedCollisionShape>> DefaultBodyShapeGenerator::buildLocalizedCollisionShapes() const {
        std::vector<std::shared_ptr<const LocalizedCollisionShape>> result;

        if (objectEntity.getModel()->getConstMeshes()) {
            const Meshes* meshes = objectEntity.getModel()->getMeshes();
            result.reserve(meshes->getNumMeshes());

            std::size_t nextShapeIndex = 0;
            for (unsigned int meshIndex = 0; meshIndex < meshes->getNumMeshes(); ++meshIndex) {
                const std::vector<Point3<float>>& vertices = meshes->getMesh(meshIndex).getVertices();
                const std::vector<std::array<uint32_t, 3>>& triangleIndices = meshes->getConstMeshes().getConstMeshes()[meshIndex]->getTrianglesIndices();
                MeshData meshData(vertices, triangleIndices);

                std::vector<std::unique_ptr<LocalizedCollisionShape>> localizedCollisionShapes = buildBestCollisionShapes(nextShapeIndex, meshData);
                for (std::unique_ptr<LocalizedCollisionShape>& localizedCollisionShape : localizedCollisionShapes) {
                    assert(nextShapeIndex == localizedCollisionShape->shapeIndex);
                    result.push_back(std::move(localizedCollisionShape));
                    nextShapeIndex++;
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

    std::vector<std::unique_ptr<LocalizedCollisionShape>> DefaultBodyShapeGenerator::buildBestCollisionShapes(std::size_t nextShapeIndex, const MeshData& mesh) const {
        MeshData simplifiedMesh = meshSimplificationService->simplify(mesh);
        std::vector<ShapeDetectService::LocalizedShape> bestLocalizedShapes = shapeDetectService->detect(simplifiedMesh);

        std::vector<std::unique_ptr<LocalizedCollisionShape>> result;
        result.reserve(bestLocalizedShapes.size());

        std::size_t shapeIndex = nextShapeIndex;
        for (ShapeDetectService::LocalizedShape& localizedShape : bestLocalizedShapes) {
            auto localizedCollisionShape = std::make_unique<LocalizedCollisionShape>();

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
            localizedCollisionShape->shapeIndex = shapeIndex++;

            result.push_back(std::move(localizedCollisionShape));
        }

        return result;
    }

}