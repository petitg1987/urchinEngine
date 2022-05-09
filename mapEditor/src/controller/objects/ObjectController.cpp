#include <stdexcept>
#include <memory>
#include <utility>

#include <controller/objects/ObjectController.h>
#include <controller/EntityControllerUtil.h>

namespace urchin {

    ObjectController::ObjectController() :
            AbstractController() {

    }

    std::list<const ObjectEntity*> ObjectController::getObjectEntities() const {
        const auto& objectEntities = getMap().getObjectEntities();
        std::list<const ObjectEntity*> constObjectEntities;
        for (auto& objectEntity : objectEntities) {
            constObjectEntities.emplace_back(objectEntity.get());
        }

        return constObjectEntities;
    }

    const ObjectEntity* ObjectController::findObjectEntityByBodyId(std::string_view bodyId) const {
        for (const auto* objectEntity : getObjectEntities()) {
            if (objectEntity->getRigidBody() && objectEntity->getRigidBody()->getId() == bodyId) {
                return objectEntity;
            }
        }

        return nullptr;
    }

    ObjectEntity& ObjectController::addObjectEntity(std::unique_ptr<ObjectEntity> objectEntity) {
        ObjectEntity& addedObjectEntity = getMap().addObjectEntity(std::move(objectEntity));

        markModified();
        return addedObjectEntity;
    }

    void ObjectController::removeObjectEntity(const ObjectEntity& constObjectEntity) {
        ObjectEntity& objectEntity = findObjectEntity(constObjectEntity);
        getMap().removeObjectEntity(objectEntity);

        markModified();
    }

    ObjectEntity& ObjectController::cloneObjectEntity(std::string newObjectName, const ObjectEntity& toCloneObjectEntity) {
        return addObjectEntity(toCloneObjectEntity.clone(std::move(newObjectName)));
    }

    void ObjectController::renameObjectEntity(const ObjectEntity& constObjectEntity, std::string newObjectName) {
        ObjectEntity& objectEntity = findObjectEntity(constObjectEntity);
        objectEntity.setName(std::move(newObjectName));

        markModified();
    }

    bool ObjectController::moveUpObjectEntity(const ObjectEntity& constObjectEntity) {
        ObjectEntity& objectEntity = findObjectEntity(constObjectEntity);
        bool moved = getMap().moveUpObjectEntity(objectEntity);

        markModified();
        return moved;
    }

    bool ObjectController::moveDownObjectEntity(const ObjectEntity& constObjectEntity) {
        ObjectEntity& objectEntity = findObjectEntity(constObjectEntity);
        bool moved = getMap().moveDownObjectEntity(objectEntity);

        markModified();
        return moved;
    }

    void ObjectController::createDefaultBody(const ObjectEntity& constObjectEntity) {
        ObjectEntity& objectEntity = findObjectEntity(constObjectEntity);
        auto rigidBody = DefaultRigidBodyGenerator(constObjectEntity).generate(CollisionShape3D::ShapeType::BOX_SHAPE);
        objectEntity.setupInteractiveBody(std::move(rigidBody));

        markModified();
    }

    void ObjectController::changeBodyShape(const ObjectEntity& constObjectEntity, CollisionShape3D::ShapeType shapeType) {
        std::unique_ptr<const CollisionShape3D> newCollisionShape = DefaultBodyShapeGenerator(constObjectEntity).generate(shapeType);
        updateObjectPhysicsShape(constObjectEntity, std::move(newCollisionShape));
    }

    void ObjectController::moveObjectInFrontOfCamera(const ObjectEntity& constObjectEntity, bool isClonedEntity) {
        Point3<float> currentPosition = constObjectEntity.getModel()->getTransform().getPosition();
        Point3<float> newPosition = EntityControllerUtil::determineClonePosition(currentPosition, isClonedEntity, getMap().getRenderer3d()->getCamera());
        constObjectEntity.updatePosition(newPosition);

        markModified();
    }

    void ObjectController::removeBody(const ObjectEntity& constObjectEntity) {
        ObjectEntity& objectEntity = findObjectEntity(constObjectEntity);
        objectEntity.setupInteractiveBody(nullptr);

        markModified();
    }

    const ObjectEntity& ObjectController::updateObjectTransform(const ObjectEntity& constObjectEntity, const Transform<float>& transform) {
        const ObjectEntity& objectEntity = findObjectEntity(constObjectEntity);
        Model* model = objectEntity.getModel();

        Transform<float> oldTransform = model->getTransform();
        model->setTransform(transform);

        if (objectEntity.getRigidBody()) {
            Vector3<float> scaleRatio = transform.getScale() / oldTransform.getScale();
            const CollisionShape3D& collisionShape = objectEntity.getRigidBody()->getShape();
            auto scaledCollisionShape = collisionShape.scale(scaleRatio);

            updateObjectPhysicsShape(objectEntity, std::move(scaledCollisionShape));
        }

        markModified();
        return objectEntity;
    }

    const ObjectEntity& ObjectController::updateObjectProperties(const ObjectEntity& constObjectEntity, Model::ShadowClass shadowClass) {
        const ObjectEntity& objectEntity = findObjectEntity(constObjectEntity);
        Model* model = objectEntity.getModel();

        model->setShadowClass(shadowClass);

        markModified();
        return objectEntity;
    }

    const ObjectEntity& ObjectController::updateObjectTags(const ObjectEntity& constObjectEntity, std::string_view tagsValues) {
        ObjectEntity& objectEntity = findObjectEntity(constObjectEntity);

        objectEntity.removeAllTags();
        std::vector<std::string> tagsList = StringUtil::split(tagsValues, TAGS_SEPARATOR);
        for (const std::string& tag: tagsList) {
            objectEntity.addTag(tag);
        }

        markModified();
        return objectEntity;
    }

    const ObjectEntity& ObjectController::updateObjectPhysicsProperties(const ObjectEntity& constObjectEntity, float mass, float restitution,
            float friction, float rollingFriction, float linearDamping, float angularDamping, const Vector3<float>& linearFactor,
            const Vector3<float>& angularFactor) {
        const ObjectEntity& objectEntity = findObjectEntity(constObjectEntity);
        RigidBody* rigidBody = objectEntity.getRigidBody();

        rigidBody->setMass(mass);
        rigidBody->setRestitution(restitution);
        rigidBody->setFriction(friction);
        rigidBody->setRollingFriction(rollingFriction);

        rigidBody->setDamping(linearDamping, angularDamping);

        rigidBody->setLinearFactor(linearFactor);
        rigidBody->setAngularFactor(angularFactor);

        markModified();
        return objectEntity;
    }

    const ObjectEntity& ObjectController::updateObjectPhysicsShape(const ObjectEntity& constObjectEntity, std::unique_ptr<const CollisionShape3D> newCollisionShape) {
        ObjectEntity& objectEntity = findObjectEntity(constObjectEntity);
        const RigidBody* rigidBody = objectEntity.getRigidBody();

        const std::string& bodyId = constObjectEntity.getName();
        Transform<float> modelTransform = constObjectEntity.getModel()->getTransform();
        PhysicsTransform physicsTransform(modelTransform.getPosition(), modelTransform.getOrientation());

        auto newRigidBody = std::make_unique<RigidBody>(bodyId, physicsTransform, std::move(newCollisionShape));
        newRigidBody->setMass(rigidBody->getMass());
        newRigidBody->setRestitution(rigidBody->getRestitution());
        newRigidBody->setFriction(rigidBody->getFriction());
        newRigidBody->setRollingFriction(rigidBody->getRollingFriction());
        newRigidBody->setDamping(rigidBody->getLinearDamping(), rigidBody->getAngularDamping());
        newRigidBody->setLinearFactor(rigidBody->getLinearFactor());
        newRigidBody->setAngularFactor(rigidBody->getAngularFactor());

        objectEntity.setupInteractiveBody(std::move(newRigidBody));

        markModified();
        return objectEntity;
    }

    ObjectEntity& ObjectController::findObjectEntity(const ObjectEntity& constObjectEntity) {
        const auto& objectEntities = getMap().getObjectEntities();
        auto it = std::ranges::find_if(objectEntities, [&constObjectEntity](const auto& o){return o.get() == &constObjectEntity;});

        if (it != objectEntities.end()) {
            return *(*it);
        }

        throw std::invalid_argument("Impossible to find object entity: " + constObjectEntity.getName());
    }

}
