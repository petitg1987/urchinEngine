#include <stdexcept>
#include <memory>
#include <UrchinMapHandler.h>

#include <controller/models/ModelController.h>
#include <panel/models/bodyshape/support/DefaultBodyShapeCreator.h>

namespace urchin {

    ModelController::ModelController() :
            AbstractController() {

    }

    std::list<const SceneModel*> ModelController::getSceneModels() const {
        const auto& sceneModels = getMapHandler()->getMap().getSceneModels();
        std::list<const SceneModel*> constSceneModels;
        for (auto& sceneModel : sceneModels) {
            constSceneModels.emplace_back(sceneModel.get());
        }

        return constSceneModels;
    }

    const SceneModel* ModelController::findSceneModelByBodyId(const std::string& bodyId) const {
        for (const auto* sceneModel : getSceneModels()) {
            if (sceneModel->getRigidBody() && sceneModel->getRigidBody()->getId() == bodyId) {
                return sceneModel;
            }
        }

        return nullptr;
    }

    void ModelController::addSceneModel(std::unique_ptr<SceneModel> sceneModel) {
        getMapHandler()->getMap().addSceneModel(std::move(sceneModel));

        markModified();
    }

    void ModelController::removeSceneModel(const SceneModel& constSceneModel) {
        SceneModel& sceneModel = findSceneModel(constSceneModel);
        getMapHandler()->getMap().removeSceneModel(sceneModel);

        markModified();
    }

    void ModelController::cloneSceneModel(std::unique_ptr<SceneModel> newSceneModel, const SceneModel& toCloneSceneModel) {
        Model* toCloneModel = toCloneSceneModel.getModel();
        auto model = std::make_shared<Model>(*toCloneModel);
        Point3<float> shiftPosition(0.5f, 0.0f, 0.0f);
        model->setPosition(model->getTransform().getPosition() + shiftPosition);
        newSceneModel->setModel(model);

        RigidBody* toCloneRigidBody = toCloneSceneModel.getRigidBody();
        if (toCloneRigidBody) {
            auto rigidBody = std::make_unique<RigidBody>(*toCloneRigidBody);
            rigidBody->setId(newSceneModel->getName());
            rigidBody->setTransform(PhysicsTransform(model->getTransform().getPosition(), model->getTransform().getOrientation()));
            newSceneModel->setupInteractiveBody(std::move(rigidBody));
        }

        addSceneModel(std::move(newSceneModel));
    }

    void ModelController::createDefaultBody(const SceneModel& constSceneModel) {
        SceneModel& sceneModel = findSceneModel(constSceneModel);

        const std::string& bodyId = constSceneModel.getName();
        Transform<float> modelTransform = constSceneModel.getModel()->getTransform();
        PhysicsTransform physicsTransform(modelTransform.getPosition(), modelTransform.getOrientation());
        auto bodyShape = DefaultBodyShapeCreator(constSceneModel).createDefaultBodyShape(CollisionShape3D::ShapeType::BOX_SHAPE);

        auto rigidBody = std::make_unique<RigidBody>(bodyId, physicsTransform, std::move(bodyShape));
        sceneModel.setupInteractiveBody(std::move(rigidBody));

        markModified();
    }

    void ModelController::changeBodyShape(const SceneModel& constSceneModel, CollisionShape3D::ShapeType shapeType) {
        std::unique_ptr<const CollisionShape3D> newCollisionShape = DefaultBodyShapeCreator(constSceneModel).createDefaultBodyShape(shapeType);
        updateSceneModelPhysicsShape(constSceneModel, std::move(newCollisionShape));
    }

    void ModelController::removeBody(const SceneModel& constSceneModel) {
        SceneModel& sceneModel = findSceneModel(constSceneModel);
        sceneModel.setupInteractiveBody(nullptr);

        markModified();
    }

    const SceneModel& ModelController::updateSceneModelTransform(const SceneModel& constSceneModel, const Transform<float>& transform) {
        SceneModel& sceneModel = findSceneModel(constSceneModel);
        Model* model = sceneModel.getModel();

        Transform<float> oldTransform = model->getTransform();
        model->setTransform(transform);

        if (sceneModel.getRigidBody()) {
            float scaleRatio = transform.getScale() / oldTransform.getScale();
            const CollisionShape3D& collisionShape = sceneModel.getRigidBody()->getShape();
            auto scaledCollisionShape = collisionShape.scale(scaleRatio);

            updateSceneModelPhysicsShape(sceneModel, std::move(scaledCollisionShape));
        }

        markModified();
        return sceneModel;
    }

    const SceneModel& ModelController::updateSceneModelFlags(const SceneModel& constSceneModel, bool produceShadow) {
        SceneModel& sceneModel = findSceneModel(constSceneModel);
        Model* model = sceneModel.getModel();

        model->setProduceShadow(produceShadow);

        markModified();
        return sceneModel;
    }

    const SceneModel& ModelController::updateSceneModelTags(const SceneModel& constSceneModel, const std::string& tagsValues) {
        SceneModel& sceneModel = findSceneModel(constSceneModel);
        Model* model = sceneModel.getModel();

        model->removeAllTags();

        std::vector<std::string> tagsList = StringUtil::split(tagsValues, ModelReaderWriter::TAGS_SEPARATOR);
        for (const std::string& tag: tagsList) {
            model->addTag(tag);
        }

        markModified();
        return sceneModel;
    }

    const SceneModel& ModelController::updateSceneModelPhysicsProperties(const SceneModel& constSceneModel, float mass, float restitution,
            float friction, float rollingFriction, float linearDamping, float angularDamping, const Vector3<float>& linearFactor,
            const Vector3<float>& angularFactor) {
        SceneModel& sceneModel = findSceneModel(constSceneModel);
        RigidBody* rigidBody = sceneModel.getRigidBody();

        rigidBody->setMass(mass);
        rigidBody->setRestitution(restitution);
        rigidBody->setFriction(friction);
        rigidBody->setRollingFriction(rollingFriction);

        rigidBody->setDamping(linearDamping, angularDamping);

        rigidBody->setLinearFactor(linearFactor);
        rigidBody->setAngularFactor(angularFactor);

        markModified();
        return sceneModel;
    }

    const SceneModel& ModelController::updateSceneModelPhysicsShape(const SceneModel& constSceneModel, std::unique_ptr<const CollisionShape3D> newCollisionShape) {
        SceneModel& sceneModel = findSceneModel(constSceneModel);
        RigidBody* rigidBody = sceneModel.getRigidBody();

        const std::string& bodyId = constSceneModel.getName();
        Transform<float> modelTransform = constSceneModel.getModel()->getTransform();
        PhysicsTransform physicsTransform(modelTransform.getPosition(), modelTransform.getOrientation());

        auto newRigidBody = std::make_unique<RigidBody>(bodyId, physicsTransform, std::move(newCollisionShape));
        newRigidBody->setMass(rigidBody->getMass());
        newRigidBody->setRestitution(rigidBody->getRestitution());
        newRigidBody->setFriction(rigidBody->getFriction());
        newRigidBody->setRollingFriction(rigidBody->getRollingFriction());
        newRigidBody->setDamping(rigidBody->getLinearDamping(), rigidBody->getAngularDamping());
        newRigidBody->setLinearFactor(rigidBody->getLinearFactor());
        newRigidBody->setAngularFactor(rigidBody->getAngularFactor());

        sceneModel.setupInteractiveBody(std::move(newRigidBody));

        markModified();
        return sceneModel;
    }

    SceneModel& ModelController::findSceneModel(const SceneModel& constSceneModel) {
        const auto& sceneModels = getMapHandler()->getMap().getSceneModels();
        auto it = std::find_if(sceneModels.begin(), sceneModels.end(), [&constSceneModel](const auto& o){return o.get() == &constSceneModel;});

        if (it != sceneModels.end()) {
            return *(*it);
        }

        throw std::invalid_argument("Impossible to find scene model: " + constSceneModel.getName());
    }

}
