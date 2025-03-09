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
                    std::unique_ptr<LocalizedCollisionShape> localizedShape = buildOptimizedCollisionShape(shapeIndex++,
                        std::vector(meshUniqueVertices.begin(), meshUniqueVertices.end()));
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

    //TODO improve + handle sphere
    std::unique_ptr<LocalizedCollisionShape> DefaultBodyShapeGenerator::buildOptimizedCollisionShape(std::size_t shapeIndex, const std::vector<Point3<float>>& uniqueVertices) const {
        auto localizedShape = std::make_unique<LocalizedCollisionShape>();
        localizedShape->shapeIndex = shapeIndex;
        localizedShape->shape = std::make_unique<const CollisionConvexHullShape>(uniqueVertices);
        localizedShape->transform = PhysicsTransform();

        std::vector<Point3<float>> points = dynamic_cast<const CollisionConvexHullShape*>(localizedShape->shape.get())->getPoints();
        if (points.size() != 8) {
            return localizedShape;
        }

        Point3<float> cornerPoint = points[0];
        std::size_t farthestPointIndex = 0;
        std::size_t closestPointIndex = 0;
        float maxDistanceToCorner = 0;
        float minDistanceToCorner = std::numeric_limits<float>::max();
        for (std::size_t i = 1; i < points.size(); ++i) {
            float distanceToCorner = cornerPoint.squareDistance(points[i]);
            if (distanceToCorner > maxDistanceToCorner) {
                maxDistanceToCorner = distanceToCorner;
                farthestPointIndex = i;
            }
            if (distanceToCorner < minDistanceToCorner) {
                minDistanceToCorner = distanceToCorner;
                closestPointIndex = i;
            }
        }

        Point3<float> boxCenterPoint = (cornerPoint + points[farthestPointIndex]) / 2.0f;
        float expectedDistanceToCenter = cornerPoint.distance(boxCenterPoint);
        float minExpectedDistanceToCenter = expectedDistanceToCenter - (expectedDistanceToCenter * 0.025f);
        float maxExpectedDistanceToCenter = expectedDistanceToCenter + (expectedDistanceToCenter * 0.025f);
        for (std::size_t i = 1; i < points.size(); ++i) {
            float distanceToCenter = points[i].distance(boxCenterPoint);
            if (distanceToCenter < minExpectedDistanceToCenter || distanceToCenter > maxExpectedDistanceToCenter) {
                return localizedShape;
            }
        }


        Vector3<float> xAxis = cornerPoint.vector(points[closestPointIndex]);

        std::array<std::size_t, 2> orthogonalVectorsToX = {0, 0};
        for (std::size_t i = 1; i < points.size(); ++i) {
            if (i != closestPointIndex || i != farthestPointIndex) {
                Vector3<float> vector = cornerPoint.vector(points[i]).normalize();
                if (xAxis.normalize().dotProduct(vector) < 0.05f) {
                    if (orthogonalVectorsToX[0] == 0) {
                        orthogonalVectorsToX[0] = i;
                    } else if (orthogonalVectorsToX[1] == 0) {
                        orthogonalVectorsToX[1] = i;
                    } else if (cornerPoint.squareDistance(points[i]) < cornerPoint.squareDistance(points[orthogonalVectorsToX[0]])) {
                        orthogonalVectorsToX[0] = i;
                    } else if (cornerPoint.squareDistance(points[i]) < cornerPoint.squareDistance(points[orthogonalVectorsToX[1]])) {
                        orthogonalVectorsToX[1] = i;
                    }
                }
            }
        }

        if (orthogonalVectorsToX[0] == 0 || orthogonalVectorsToX[1] == 0) {
            return localizedShape;
        }

        Vector3<float> yAxis = cornerPoint.vector(points[orthogonalVectorsToX[0]]);
        Vector3<float> zAxis = cornerPoint.vector(points[orthogonalVectorsToX[1]]);
        Quaternion<float> xOrientation = Quaternion<float>::rotationFromTo(xAxis.normalize(), Vector3(1.0f, 0.0f, 0.0f)).normalize();
        Vector3<float> yAxisRotated = xOrientation.rotateVector(yAxis.normalize());
        Quaternion<float> yOrientation = Quaternion<float>::rotationFromTo(yAxisRotated.normalize(), Vector3(0.0f, 1.0f, 0.0f)).normalize();

        Vector3 halfSize(xAxis.length() / 2.0f, yAxis.length() / 2.0f, zAxis.length() / 2.0f);
        localizedShape->shape = std::make_unique<const CollisionBoxShape>(halfSize);
        localizedShape->transform = PhysicsTransform(boxCenterPoint, (xOrientation * yOrientation).normalize()); //TODO norm useful ?
        return localizedShape;
    }

}