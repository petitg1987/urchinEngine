#include <stdexcept>
#include <memory>

#include <controller/objects/ObjectController.h>
#include <panel/objects/bodyshape/support/DefaultBodyShapeCreator.h>

namespace urchin {

    ObjectController::ObjectController() :
            AbstractController() {

    }

    std::list<const SceneObject*> ObjectController::getSceneObjects() const {
        const auto& sceneObjects = getMapHandler()->getMap().getSceneObjects();
        std::list<const SceneObject*> constSceneObjects;
        for(auto& sceneObject : sceneObjects) {
            constSceneObjects.emplace_back(sceneObject.get());
        }

        return constSceneObjects;
    }

    const SceneObject* ObjectController::findSceneObjectByBodyId(const std::string& bodyId) const {
        for (const auto* sceneObject : getSceneObjects()) {
            if (sceneObject->getRigidBody() && sceneObject->getRigidBody()->getId() == bodyId) {
                return sceneObject;
            }
        }

        return nullptr;
    }

    void ObjectController::addSceneObject(std::unique_ptr<SceneObject> sceneObject) {
        getMapHandler()->getMap().addSceneObject(std::move(sceneObject));

        markModified();
    }

    void ObjectController::removeSceneObject(const SceneObject& constSceneObject) {
        SceneObject& sceneObject = findSceneObject(constSceneObject);
        getMapHandler()->getMap().removeSceneObject(sceneObject);

        markModified();
    }

    void ObjectController::cloneSceneObject(std::unique_ptr<SceneObject> newSceneObject, const SceneObject& toCloneSceneObject) {
        Model* toCloneModel = toCloneSceneObject.getModel();
        auto model = std::make_shared<Model>(*toCloneModel);
        Point3<float> shiftPosition(0.5f, 0.0f, 0.0f);
        model->setPosition(model->getTransform().getPosition() + shiftPosition);
        newSceneObject->setModel(model);

        RigidBody* toCloneRigidBody = toCloneSceneObject.getRigidBody();
        if (toCloneRigidBody) {
            auto rigidBody = std::make_unique<RigidBody>(*toCloneRigidBody);
            rigidBody->setId(newSceneObject->getName());
            rigidBody->setTransform(PhysicsTransform(model->getTransform().getPosition(), model->getTransform().getOrientation()));
            newSceneObject->setupInteractiveBody(std::move(rigidBody));
        }

        addSceneObject(std::move(newSceneObject));
    }

    void ObjectController::createDefaultBody(const SceneObject& constSceneObject) {
        SceneObject& sceneObject = findSceneObject(constSceneObject);

        const std::string& bodyId = constSceneObject.getName();
        Transform<float> modelTransform = constSceneObject.getModel()->getTransform();
        PhysicsTransform physicsTransform(modelTransform.getPosition(), modelTransform.getOrientation());
        auto bodyShape = DefaultBodyShapeCreator(constSceneObject).createDefaultBodyShape(CollisionShape3D::ShapeType::BOX_SHAPE);

        auto rigidBody = std::make_unique<RigidBody>(bodyId, physicsTransform, std::move(bodyShape));
        sceneObject.setupInteractiveBody(std::move(rigidBody));

        markModified();
    }

    void ObjectController::changeBodyShape(const SceneObject& constSceneObject, CollisionShape3D::ShapeType shapeType) {
        std::unique_ptr<const CollisionShape3D> newCollisionShape = DefaultBodyShapeCreator(constSceneObject).createDefaultBodyShape(shapeType);
        updateSceneObjectPhysicsShape(constSceneObject, std::move(newCollisionShape));
    }

    void ObjectController::removeBody(const SceneObject& constSceneObject) {
        SceneObject& sceneObject = findSceneObject(constSceneObject);
        sceneObject.setupInteractiveBody(nullptr);

        markModified();
    }

    const SceneObject& ObjectController::updateSceneObjectTransform(const SceneObject& constSceneObject, const Transform<float>& transform) {
        SceneObject& sceneObject = findSceneObject(constSceneObject);
        Model* model = sceneObject.getModel();

        Transform<float> oldTransform = model->getTransform();
        model->setTransform(transform);

        if (sceneObject.getRigidBody()) {
            float scaleRatio = transform.getScale() / oldTransform.getScale();
            const CollisionShape3D& collisionShape = sceneObject.getRigidBody()->getShape();
            auto scaledCollisionShape = collisionShape.scale(scaleRatio);

            updateSceneObjectPhysicsShape(sceneObject, std::move(scaledCollisionShape));
        }

        markModified();
        return sceneObject;
    }

    const SceneObject& ObjectController::updateSceneObjectFlags(const SceneObject& constSceneObject, bool produceShadow) {
        SceneObject& sceneObject = findSceneObject(constSceneObject);
        Model* model = sceneObject.getModel();

        model->setProduceShadow(produceShadow);

        markModified();
        return sceneObject;
    }

    const SceneObject& ObjectController::updateSceneObjectPhysicsProperties(const SceneObject& constSceneObject, float mass, float restitution,
            float friction, float rollingFriction, float linearDamping, float angularDamping, const Vector3<float>& linearFactor,
            const Vector3<float>& angularFactor) {
        SceneObject& sceneObject = findSceneObject(constSceneObject);
        RigidBody* rigidBody = sceneObject.getRigidBody();

        rigidBody->setMass(mass);
        rigidBody->setRestitution(restitution);
        rigidBody->setFriction(friction);
        rigidBody->setRollingFriction(rollingFriction);

        rigidBody->setDamping(linearDamping, angularDamping);

        rigidBody->setLinearFactor(linearFactor);
        rigidBody->setAngularFactor(angularFactor);

        markModified();
        return sceneObject;
    }

    const SceneObject& ObjectController::updateSceneObjectPhysicsShape(const SceneObject& constSceneObject, std::unique_ptr<const CollisionShape3D> newCollisionShape) {
        SceneObject& sceneObject = findSceneObject(constSceneObject);
        RigidBody* rigidBody = sceneObject.getRigidBody();

        const std::string& bodyId = constSceneObject.getName();
        Transform<float> modelTransform = constSceneObject.getModel()->getTransform();
        PhysicsTransform physicsTransform(modelTransform.getPosition(), modelTransform.getOrientation());

        auto newRigidBody = std::make_unique<RigidBody>(bodyId, physicsTransform, std::move(newCollisionShape));
        newRigidBody->setMass(rigidBody->getMass());
        newRigidBody->setRestitution(rigidBody->getRestitution());
        newRigidBody->setFriction(rigidBody->getFriction());
        newRigidBody->setRollingFriction(rigidBody->getRollingFriction());
        newRigidBody->setDamping(rigidBody->getLinearDamping(), rigidBody->getAngularDamping());
        newRigidBody->setLinearFactor(rigidBody->getLinearFactor());
        newRigidBody->setAngularFactor(rigidBody->getAngularFactor());

        sceneObject.setupInteractiveBody(std::move(newRigidBody));

        markModified();
        return sceneObject;
    }

    SceneObject& ObjectController::findSceneObject(const SceneObject& constSceneObject) {
        const auto& sceneObjects = getMapHandler()->getMap().getSceneObjects();
        auto it = std::find_if(sceneObjects.begin(), sceneObjects.end(), [&constSceneObject](const auto& o){return o.get() == &constSceneObject;});

        if (it != sceneObjects.end()) {
            return *(*it);
        }

        throw std::invalid_argument("Impossible to find scene object: " + constSceneObject.getName());
    }

}
