#include <stdexcept>
#include <AIEnvironment.h>

#include <resources/object/ObjectEntity.h>
#include <resources/common/AIEntityBuilder.h>

namespace urchin {

    ObjectEntity::ObjectEntity() :
            renderer3d(nullptr),
            physicsWorld(nullptr),
            aiEnvironment(nullptr),
            rigidBody(nullptr),
            aiObject(nullptr) {

    }

    ObjectEntity::~ObjectEntity() {
        renderer3d->removeModel(model.get());
        deleteRigidBody();
        deleteAIObjects();
    }

    void ObjectEntity::setup(Renderer3d* renderer3d, PhysicsWorld* physicsWorld, AIEnvironment* aiEnvironment) {
        if (this->renderer3d) {
            throw std::invalid_argument("Cannot add the object entity on two different renderer 3d.");
        }
        if (!renderer3d) {
            throw std::invalid_argument("Cannot specify a null renderer 3d for an object entity.");
        }

        this->renderer3d = renderer3d;
        this->physicsWorld = physicsWorld;
        this->aiEnvironment = aiEnvironment;

        renderer3d->addModel(model);

        if (physicsWorld && rigidBody) {
            physicsWorld->addBody(rigidBody);
        }

        if (aiEnvironment && aiObject) {
            aiEnvironment->addEntity(aiObject);
        }
    }

    const std::string& ObjectEntity::getName() const {
        return name;
    }

    void ObjectEntity::setName(const std::string& name) {
        this->name = name;
    }

    Model* ObjectEntity::getModel() const {
        return model.get();
    }

    void ObjectEntity::setModel(const std::shared_ptr<Model>& model) {
        if (!model) {
            throw std::invalid_argument("Cannot set a null model on an object entity.");
        }

        if (renderer3d) {
            renderer3d->removeModel(this->model.get());
            renderer3d->addModel(model);
        }

        this->model = model;
    }

    void ObjectEntity::setupInteractiveBody(const std::shared_ptr<RigidBody>& rigidBody) {
        setupRigidBody(rigidBody);
        setupAIObject();
    }

    RigidBody* ObjectEntity::getRigidBody() const {
        return rigidBody.get();
    }

    void ObjectEntity::setupRigidBody(const std::shared_ptr<RigidBody>& rigidBody) {
        deleteRigidBody();

        this->rigidBody = rigidBody;
        if (physicsWorld && rigidBody) {
            physicsWorld->addBody(rigidBody);
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

    void ObjectEntity::refresh() {
        if (rigidBody && rigidBody->isActive()) {
            PhysicsTransform physicsTransform = rigidBody->getTransform();
            model->setTransform(Transform(physicsTransform.getPosition(), physicsTransform.getOrientation(), model->getTransform().getScale()));
            if (aiObject) {
                aiObject->updateTransform(physicsTransform.getPosition(), physicsTransform.getOrientation());
            }
        }
    }

}
