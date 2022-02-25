#include <stdexcept>
#include <AIEnvironment.h>

#include <model/ObjectEntity.h>
#include <model/builder/AIEntityBuilder.h>

namespace urchin {

    ObjectEntity::ObjectEntity() :
            renderer3d(nullptr),
            physicsWorld(nullptr),
            aiEnvironment(nullptr),
            rigidBody(nullptr),
            aiObject(nullptr) {

    }

    ObjectEntity::~ObjectEntity() {
        if (renderer3d) {
            renderer3d->removeModel(model.get());
        }
        deleteRigidBody();
        deleteAIObjects();
    }

    void ObjectEntity::setup(Renderer3d& renderer3d, PhysicsWorld& physicsWorld, AIEnvironment& aiEnvironment) {
        this->renderer3d = &renderer3d;
        this->physicsWorld = &physicsWorld;
        this->aiEnvironment = &aiEnvironment;

        renderer3d.addModel(model);
        if (rigidBody) {
            physicsWorld.addBody(rigidBody);
        }
        if (aiObject) {
            aiEnvironment.addEntity(aiObject);
        }
    }

    void ObjectEntity::setName(std::string name) {
        this->name = std::move(name);
    }

    const std::string& ObjectEntity::getName() const {
        return name;
    }

    void ObjectEntity::setModel(const std::shared_ptr<Model>& model) {
        if (renderer3d) {
            renderer3d->removeModel(this->model.get());
            renderer3d->addModel(model);
        }
        this->model = model;
    }

    Model* ObjectEntity::getModel() const {
        return model.get();
    }

    const std::shared_ptr<Model>& ObjectEntity::getModelPtr() const {
        return model;
    }

    void ObjectEntity::setupInteractiveBody(std::shared_ptr<RigidBody> rigidBody) {
        setupRigidBody(std::move(rigidBody));
        setupAIObject();
    }

    RigidBody* ObjectEntity::getRigidBody() const {
        return rigidBody.get();
    }

    const std::shared_ptr<RigidBody>& ObjectEntity::getRigidBodyPtr() const {
        return rigidBody;
    }

    void ObjectEntity::setupRigidBody(std::shared_ptr<RigidBody> rigidBody) {
        deleteRigidBody();

        this->rigidBody = std::move(rigidBody);
        if (physicsWorld && this->rigidBody) {
            physicsWorld->addBody(this->rigidBody);
        }
    }

    void ObjectEntity::setupAIObject() {
        deleteAIObjects();

        if (!rigidBody || !model->getConstMeshes()) {
            this->aiObject = nullptr;
        } else {
            std::string aiObjectName = "#" + rigidBody->getId(); //prefix to avoid collision name with terrains
            this->aiObject = AIEntityBuilder::buildAIObject(aiObjectName, rigidBody->getShape(), rigidBody->getTransform().toTransform());
            if (aiEnvironment) {
                aiEnvironment->addEntity(aiObject);
            }
        }
    }

    void ObjectEntity::deleteRigidBody() {
        if (physicsWorld && rigidBody) {
            physicsWorld->removeBody(*rigidBody);
        }
        rigidBody = nullptr;
    }

    void ObjectEntity::deleteAIObjects() {
        if (aiEnvironment && aiObject) {
            aiEnvironment->removeEntity(aiObject);
        }
    }

    void ObjectEntity::refresh() const {
        if (rigidBody && rigidBody->isActive()) {
            PhysicsTransform physicsTransform = rigidBody->getTransform();
            model->setTransform(Transform(physicsTransform.getPosition(), physicsTransform.getOrientation(), model->getTransform().getScale()));
            if (aiObject) {
                aiObject->updateTransform(physicsTransform.getPosition(), physicsTransform.getOrientation());
            }
        }
    }

}
