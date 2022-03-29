#include <stdexcept>
#include <UrchinCommon.h>

#include <scene/objects/BodyShapeDisplayer.h>

namespace urchin {

    BodyShapeDisplayer::BodyShapeDisplayer(Scene& scene) :
            scene(scene),
            selectedObjectEntity(nullptr),
            selectedCompoundShapeComponent(nullptr) {

    }

    BodyShapeDisplayer::~BodyShapeDisplayer() {
        clearDisplay();
    }

    void BodyShapeDisplayer::setSelectedObjectEntity(const ObjectEntity* selectedObjectEntity) {
        this->selectedObjectEntity = selectedObjectEntity;
    }

    void BodyShapeDisplayer::setSelectedCompoundShapeComponent(const LocalizedCollisionShape* selectedCompoundShapeComponent) {
        this->selectedCompoundShapeComponent = selectedCompoundShapeComponent;
    }

    void BodyShapeDisplayer::displayBodyShape() {
        clearDisplay();

        if (selectedObjectEntity && selectedObjectEntity->getRigidBody()) {
            const Transform<float>& modelTransform = selectedObjectEntity->getModel()->getTransform();
            const CollisionShape3D& bodyShape = selectedObjectEntity->getRigidBody()->getShape();

            if (bodyShape.isConcave()) {
                PhysicsTransform transform(modelTransform.getPosition(), modelTransform.getOrientation());
                AABBox<float> heightfieldAABBox = bodyShape.toAABBox(transform);

                auto geometryModel = std::make_unique<AABBoxModel>(heightfieldAABBox);
                geometryModel->setColor(0.0f, 1.0f, 0.0f);
                bodyShapeModels.push_back(std::move(geometryModel));
            } else if (bodyShape.isCompound()) {
                const auto& compoundShape = static_cast<const CollisionCompoundShape&>(bodyShape);
                const std::vector<std::shared_ptr<const LocalizedCollisionShape>>& localizedShapes = compoundShape.getLocalizedShapes();
                for (const auto& localizedShape : localizedShapes) {
                    PhysicsTransform transform = PhysicsTransform(modelTransform.getPosition(), modelTransform.getOrientation()) * localizedShape->transform;
                    std::unique_ptr<CollisionConvexObject3D, ObjectDeleter> bodyObject = localizedShape->shape->toConvexObject(transform);

                    auto geometryModel = retrieveSingleGeometry(localizedShape->shape->getShapeType(), *bodyObject);
                    if (selectedCompoundShapeComponent != nullptr && selectedCompoundShapeComponent->shapeIndex == localizedShape->shapeIndex) {
                        geometryModel->setColor(1.0f, 1.0f, 1.0f);
                    } else {
                        geometryModel->setColor(0.0f, 1.0f, 0.0f);
                    }
                    bodyShapeModels.push_back(std::move(geometryModel));
                }
            } else if (bodyShape.isConvex()) {
                PhysicsTransform transform(modelTransform.getPosition(), modelTransform.getOrientation());
                std::unique_ptr<CollisionConvexObject3D, ObjectDeleter> bodyObject = bodyShape.toConvexObject(transform);

                auto geometryModel = retrieveSingleGeometry(bodyShape.getShapeType(), *bodyObject);
                geometryModel->setColor(0.0f, 1.0f, 0.0f);
                bodyShapeModels.push_back(std::move(geometryModel));
            } else {
                throw std::invalid_argument("Unknown shape type category: " + std::to_string(bodyShape.getShapeType()));
            }

            for (const auto& bodyShapeModel : bodyShapeModels) {
                scene.getActiveRenderer3d()->getGeometryContainer().addGeometry(bodyShapeModel);
            }
        }
    }

    std::unique_ptr<GeometryModel> BodyShapeDisplayer::retrieveSingleGeometry(CollisionShape3D::ShapeType shapeType, const CollisionConvexObject3D& bodyObject) {
        if (shapeType == CollisionShape3D::SPHERE_SHAPE) {
            return retrieveSphereGeometry(bodyObject);
        }
        if (shapeType == CollisionShape3D::BOX_SHAPE) {
            return retrieveBoxGeometry(bodyObject);
        }
        if (shapeType == CollisionShape3D::CYLINDER_SHAPE) {
            return retrieveCylinderGeometry(bodyObject);
        }
        if (shapeType == CollisionShape3D::CONE_SHAPE) {
            return retrieveConeGeometry(bodyObject);
        }
        if (shapeType == CollisionShape3D::CAPSULE_SHAPE) {
            return retrieveCapsuleGeometry(bodyObject);
        }
        if (shapeType == CollisionShape3D::CONVEX_HULL_SHAPE) {
            return retrieveConvexHullGeometry(bodyObject);
        }
        if (shapeType == CollisionShape3D::COMPOUND_SHAPE) {
            throw std::invalid_argument("Impossible to return a simple geometry from a compound shape");
        }

        throw std::invalid_argument("Unknown shape type to retrieve geometry: " + std::to_string(shapeType));
    }

    std::unique_ptr<GeometryModel> BodyShapeDisplayer::retrieveSphereGeometry(const CollisionConvexObject3D& bodyObject) {
        const auto& sphereObject = static_cast<const CollisionSphereObject&>(bodyObject);
        return std::make_unique<SphereModel>(sphereObject.retrieveSphere(), 15);
    }

    std::unique_ptr<GeometryModel> BodyShapeDisplayer::retrieveBoxGeometry(const CollisionConvexObject3D& bodyObject) {
        const auto& boxObject = static_cast<const CollisionBoxObject&>(bodyObject);
        return std::make_unique<OBBoxModel>(boxObject.retrieveOBBox());
    }

    std::unique_ptr<GeometryModel> BodyShapeDisplayer::retrieveCylinderGeometry(const CollisionConvexObject3D& bodyObject) {
        const auto& cylinderObject = static_cast<const CollisionCylinderObject&>(bodyObject);
        return std::make_unique<CylinderModel>(cylinderObject.retrieveCylinder(), 15);
    }

    std::unique_ptr<GeometryModel> BodyShapeDisplayer::retrieveConeGeometry(const CollisionConvexObject3D& bodyObject) {
        const auto& coneObject = static_cast<const CollisionConeObject&>(bodyObject);
        return std::make_unique<ConeModel>(coneObject.retrieveCone(), 15);
    }

    std::unique_ptr<GeometryModel> BodyShapeDisplayer::retrieveCapsuleGeometry(const CollisionConvexObject3D& bodyObject) {
        const auto& capsuleObject = static_cast<const CollisionCapsuleObject&>(bodyObject);
        return std::make_unique<CapsuleModel>(capsuleObject.retrieveCapsule(), 15, 15);
    }

    std::unique_ptr<GeometryModel> BodyShapeDisplayer::retrieveConvexHullGeometry(const CollisionConvexObject3D& bodyObject) {
        const auto& convexHullObject = static_cast<const CollisionConvexHullObject&>(bodyObject);
        return std::make_unique<ConvexHullModel>(convexHullObject.getConvexHullWithMargin());
    }

    void BodyShapeDisplayer::clearDisplay() {
        for (const auto& bodyShapeModel : bodyShapeModels) {
            scene.getActiveRenderer3d()->getGeometryContainer().removeGeometry(*bodyShapeModel);
        }
        bodyShapeModels.clear();
    }
}
