#include <stdexcept>
#include <UrchinCommon.h>

#include "scene/object/physics/BodyShapeDisplayer.h"

namespace urchin {

    BodyShapeDisplayer::BodyShapeDisplayer(Scene& scene) :
            scene(scene),
            selectedCompoundShapeComponent(nullptr) {

    }

    BodyShapeDisplayer::~BodyShapeDisplayer() {
        clearDisplay();
    }

    void BodyShapeDisplayer::setSelectedObjectEntities(std::vector<const ObjectEntity*> selectedObjectEntity) {
        this->selectedObjectEntities = std::move(selectedObjectEntity);
    }

    void BodyShapeDisplayer::setSelectedCompoundShapeComponent(const LocalizedCollisionShape* selectedCompoundShapeComponent) {
        this->selectedCompoundShapeComponent = selectedCompoundShapeComponent;
    }

    void BodyShapeDisplayer::displayBodyShape() {
        clearDisplay();

        for (const ObjectEntity* selectedObjectEntity : selectedObjectEntities) {
            if (!selectedObjectEntity->getModel() || !selectedObjectEntity->getRigidBody()) {
                continue;
            }

            const Transform<float>& modelTransform = selectedObjectEntity->getModel()->getTransform();
            const CollisionShape3D& bodyShape = selectedObjectEntity->getRigidBody()->getShape();

            Vector3 activeObjectColor(0.2f, 0.8f, 0.2f);
            Vector3 inactiveObjectColor(0.0f, 0.3f, 0.0f);

            if (bodyShape.isConcave()) {
                PhysicsTransform transform(modelTransform.getPosition(), modelTransform.getOrientation());
                AABBox<float> heightfieldAABBox = bodyShape.toAABBox(transform);

                auto geometryModel = std::make_unique<AABBoxModel>(heightfieldAABBox);
                geometryModel->setColor(activeObjectColor);
                geometryModel->setPolygonMode(PolygonMode::WIREFRAME);
                bodyShapeModels.push_back(std::move(geometryModel));
            } else if (bodyShape.isCompound()) {
                const auto& compoundShape = static_cast<const CollisionCompoundShape&>(bodyShape);
                const std::vector<std::shared_ptr<const LocalizedCollisionShape>>& localizedShapes = compoundShape.getLocalizedShapes();
                for (const auto& localizedShape : localizedShapes) {
                    PhysicsTransform transform = PhysicsTransform(modelTransform.getPosition(), modelTransform.getOrientation()) * localizedShape->transform;
                    std::unique_ptr<CollisionConvexObject3D, ObjectDeleter> bodyObject = localizedShape->shape->toConvexObject(transform);

                    auto geometryModel = retrieveSingleGeometry(localizedShape->shape->getShapeType(), *bodyObject);
                    if (selectedCompoundShapeComponent != nullptr && selectedCompoundShapeComponent->shapeIndex == localizedShape->shapeIndex) {
                        geometryModel->setColor(activeObjectColor);
                    } else {
                        geometryModel->setColor(inactiveObjectColor);
                    }
                    geometryModel->setPolygonMode(PolygonMode::WIREFRAME);
                    bodyShapeModels.push_back(std::move(geometryModel));
                }
            } else if (bodyShape.isConvex()) {
                PhysicsTransform transform(modelTransform.getPosition(), modelTransform.getOrientation());
                std::unique_ptr<CollisionConvexObject3D, ObjectDeleter> bodyObject = bodyShape.toConvexObject(transform);

                auto geometryModel = retrieveSingleGeometry(bodyShape.getShapeType(), *bodyObject);
                geometryModel->setColor(activeObjectColor);
                geometryModel->setPolygonMode(PolygonMode::WIREFRAME);
                bodyShapeModels.push_back(std::move(geometryModel));
            } else {
                throw std::invalid_argument("Unknown shape type category: " + std::to_string(bodyShape.getShapeType()));
            }
        }

        for (const auto& bodyShapeModel : bodyShapeModels) {
            scene.getActiveRenderer3d()->getGeometryContainer().addGeometry(bodyShapeModel);
        }
    }

    std::unique_ptr<GeometryModel> BodyShapeDisplayer::retrieveSingleGeometry(CollisionShape3D::ShapeType shapeType, const CollisionConvexObject3D& bodyObject) const {
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

    std::unique_ptr<GeometryModel> BodyShapeDisplayer::retrieveSphereGeometry(const CollisionConvexObject3D& bodyObject) const {
        const auto& sphereObject = static_cast<const CollisionSphereObject&>(bodyObject);
        return std::make_unique<SphereModel>(sphereObject.retrieveSphere(), 15);
    }

    std::unique_ptr<GeometryModel> BodyShapeDisplayer::retrieveBoxGeometry(const CollisionConvexObject3D& bodyObject) const {
        const auto& boxObject = static_cast<const CollisionBoxObject&>(bodyObject);
        return std::make_unique<OBBoxModel>(boxObject.retrieveOBBox());
    }

    std::unique_ptr<GeometryModel> BodyShapeDisplayer::retrieveCylinderGeometry(const CollisionConvexObject3D& bodyObject) const {
        const auto& cylinderObject = static_cast<const CollisionCylinderObject&>(bodyObject);
        return std::make_unique<CylinderModel>(cylinderObject.retrieveCylinder(), 15);
    }

    std::unique_ptr<GeometryModel> BodyShapeDisplayer::retrieveConeGeometry(const CollisionConvexObject3D& bodyObject) const {
        const auto& coneObject = static_cast<const CollisionConeObject&>(bodyObject);
        return std::make_unique<ConeModel>(coneObject.retrieveCone(), 15);
    }

    std::unique_ptr<GeometryModel> BodyShapeDisplayer::retrieveCapsuleGeometry(const CollisionConvexObject3D& bodyObject) const {
        const auto& capsuleObject = static_cast<const CollisionCapsuleObject&>(bodyObject);
        return std::make_unique<CapsuleModel>(capsuleObject.retrieveCapsule(), 15, 15);
    }

    std::unique_ptr<GeometryModel> BodyShapeDisplayer::retrieveConvexHullGeometry(const CollisionConvexObject3D& bodyObject) const {
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
