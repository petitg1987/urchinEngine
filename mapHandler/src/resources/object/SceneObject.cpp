#include <stdexcept>
#include <AIManager.h>

#include "SceneObject.h"
#include <resources/object/ModelReaderWriter.h>
#include <resources/object/RigidBodyReaderWriter.h>
#include <resources/common/AIEntityBuilder.h>

namespace urchin {

    SceneObject::SceneObject() :
            renderer3d(nullptr),
            physicsWorld(nullptr),
            aiManager(nullptr),
            model(nullptr),
            rigidBody(nullptr),
            aiObject(nullptr) {

    }

    SceneObject::~SceneObject() {
        renderer3d->removeModel(model);
        deleteRigidBody();
        deleteAIObjects();
    }

    void SceneObject::setObjectManagers(Renderer3d* renderer3d, PhysicsWorld* physicsWorld, AIManager* aiManager) {
        if (this->renderer3d) {
            throw std::invalid_argument("Cannot add the scene object on two different object managers.");
        }
        if (!renderer3d) {
            throw std::invalid_argument("Cannot specify a null renderer 3d for a scene object.");
        }

        this->renderer3d = renderer3d;
        this->physicsWorld = physicsWorld;
        this->aiManager = aiManager;

        renderer3d->addModel(model);

        if (physicsWorld && rigidBody) {
            physicsWorld->addBody(rigidBody);
        }

        if (aiManager && aiObject) {
            aiManager->addEntity(aiObject);
        }
    }

    void SceneObject::loadFrom(const UdaChunk* chunk, const DataParser& dataParser) {
        this->name = chunk->getAttributeValue(NAME_ATTR);

        auto modelChunk = dataParser.getUniqueChunk(true, MODEL_TAG, UdaAttribute(), chunk);
        setModel(ModelReaderWriter::loadFrom(modelChunk, dataParser));

        auto physicsChunk = dataParser.getUniqueChunk(false, PHYSICS_TAG, UdaAttribute(), chunk);
        if (physicsChunk != nullptr) {
            std::string rigidBodyId = this->name;
            const Transform<float>& modelTransform = this->model->getTransform();

            setupInteractiveBody(RigidBodyReaderWriter::loadFrom(physicsChunk, rigidBodyId, modelTransform, dataParser));
        }
    }

    void SceneObject::writeOn(UdaChunk& chunk, UdaWriter& udaWriter) const {
        chunk.addAttribute(UdaAttribute(NAME_ATTR, this->name));

        auto& modelChunk = udaWriter.createChunk(MODEL_TAG, UdaAttribute(), &chunk);
        ModelReaderWriter::writeOn(modelChunk, model, udaWriter);

        if (rigidBody) {
            auto& physicsChunk = udaWriter.createChunk(PHYSICS_TAG, UdaAttribute(), &chunk);
            RigidBodyReaderWriter::writeOn(physicsChunk, rigidBody, udaWriter);
        }
    }

    const std::string& SceneObject::getName() const {
        return name;
    }

    void SceneObject::setName(const std::string& name) {
        this->name = name;
    }

    Model* SceneObject::getModel() const {
        return model;
    }

    void SceneObject::setModel(Model* model) {
        if (!model) {
            throw std::invalid_argument("Cannot set a null model on scene object.");
        }

        if (renderer3d) {
            renderer3d->removeModel(this->model);
            renderer3d->addModel(model);
        } else {
            delete this->model;
        }

        this->model = model;
    }

    void SceneObject::setupInteractiveBody(RigidBody* rigidBody) {
        setupRigidBody(rigidBody);
        setupAIObject();
    }

    RigidBody* SceneObject::getRigidBody() const {
        return rigidBody;
    }

    void SceneObject::moveTo(const Point3<float>& position, const Quaternion<float>& orientation) {
        model->setTransform(Transform(position, orientation, model->getTransform().getScale()));
        if (aiObject) {
            aiObject->updateTransform(position, orientation);
        }
    }

    void SceneObject::setupRigidBody(RigidBody* rigidBody) {
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
            if (aiManager) {
                aiManager->addEntity(aiObject);
            }
        }
    }

    void SceneObject::deleteRigidBody() {
        if (physicsWorld && rigidBody) {
            physicsWorld->removeBody(rigidBody);
        } else {
            delete rigidBody;
        }

        rigidBody = nullptr;
    }

    void SceneObject::deleteAIObjects() {
        if (aiManager && aiObject) {
            aiManager->removeEntity(aiObject);
        }
    }

}
