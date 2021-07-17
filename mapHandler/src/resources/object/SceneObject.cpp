#include <stdexcept>
#include <AIEnvironment.h>

#include "SceneObject.h"
#include <resources/object/ModelReaderWriter.h>
#include <resources/object/RigidBodyReaderWriter.h>
#include <resources/common/AIEntityBuilder.h>

namespace urchin {

    SceneObject::SceneObject() :
            renderer3d(nullptr),
            physicsWorld(nullptr),
            aiEnvironment(nullptr),
            rigidBody(nullptr),
            aiObject(nullptr) {

    }

    SceneObject::~SceneObject() {
        renderer3d->removeModel(model.get());
        deleteRigidBody();
        deleteAIObjects();
    }

    void SceneObject::setObjectManagers(Renderer3d* renderer3d, PhysicsWorld* physicsWorld, AIEnvironment* aiEnvironment) {
        if (this->renderer3d) {
            throw std::invalid_argument("Cannot add the scene object on two different object managers.");
        }
        if (!renderer3d) {
            throw std::invalid_argument("Cannot specify a null renderer 3d for a scene object.");
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

    void SceneObject::loadFrom(const UdaChunk* chunk, const UdaParser& udaParser) {
        this->name = chunk->getAttributeValue(NAME_ATTR);

        auto modelChunk = udaParser.getUniqueChunk(true, MODEL_TAG, UdaAttribute(), chunk);
        setModel(ModelReaderWriter::loadFrom(modelChunk, udaParser));

        auto physicsChunk = udaParser.getUniqueChunk(false, PHYSICS_TAG, UdaAttribute(), chunk);
        if (physicsChunk != nullptr) {
            std::string rigidBodyId = this->name;
            const Transform<float>& modelTransform = this->model->getTransform();

            setupInteractiveBody(RigidBodyReaderWriter::loadFrom(physicsChunk, rigidBodyId, modelTransform, udaParser));
        }
    }

    void SceneObject::writeOn(UdaChunk& chunk, UdaWriter& udaWriter) const {
        chunk.addAttribute(UdaAttribute(NAME_ATTR, this->name));

        auto& modelChunk = udaWriter.createChunk(MODEL_TAG, UdaAttribute(), &chunk);
        ModelReaderWriter::writeOn(modelChunk, *model, udaWriter);

        if (rigidBody) {
            auto& physicsChunk = udaWriter.createChunk(PHYSICS_TAG, UdaAttribute(), &chunk);
            RigidBodyReaderWriter::writeOn(physicsChunk, *rigidBody, udaWriter);
        }
    }

    const std::string& SceneObject::getName() const {
        return name;
    }

    void SceneObject::setName(const std::string& name) {
        this->name = name;
    }

    Model* SceneObject::getModel() const {
        return model.get();
    }

    void SceneObject::setModel(const std::shared_ptr<Model>& model) {
        if (!model) {
            throw std::invalid_argument("Cannot set a null model on scene object.");
        }

        if (renderer3d) {
            renderer3d->removeModel(this->model.get());
            renderer3d->addModel(model);
        }

        this->model = model;
    }

    void SceneObject::setupInteractiveBody(const std::shared_ptr<RigidBody>& rigidBody) {
        setupRigidBody(rigidBody);
        setupAIObject();
    }

    RigidBody* SceneObject::getRigidBody() const {
        return rigidBody.get();
    }

    void SceneObject::moveTo(const Point3<float>& position, const Quaternion<float>& orientation) {
        model->setTransform(Transform(position, orientation, model->getTransform().getScale()));
        if (aiObject) {
            aiObject->updateTransform(position, orientation);
        }
    }

    void SceneObject::setupRigidBody(const std::shared_ptr<RigidBody>& rigidBody) {
        deleteRigidBody();

        this->rigidBody = rigidBody;
        if (physicsWorld && rigidBody) {
            physicsWorld->addBody(rigidBody);
        }
    }

    void SceneObject::setupAIObject() {
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

    void SceneObject::deleteRigidBody() {
        if (physicsWorld && rigidBody) {
            physicsWorld->removeBody(*rigidBody);
        }
        rigidBody = nullptr;
    }

    void SceneObject::deleteAIObjects() {
        if (aiEnvironment && aiObject) {
            aiEnvironment->removeEntity(aiObject);
        }
    }

}
