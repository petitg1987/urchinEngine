#include <stdexcept>
#include <AIManager.h>

#include "SceneObject.h"
#include "resources/object/ModelReaderWriter.h"
#include "resources/object/RigidBodyReaderWriter.h"
#include "resources/common/AIEntityBuilder.h"

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

    void SceneObject::loadFrom(const std::shared_ptr<XmlChunk>& chunk, const XmlParser& xmlParser) {
        this->name = chunk->getAttributeValue(NAME_ATTR);

        std::shared_ptr<XmlChunk> modelChunk = xmlParser.getUniqueChunk(true, MODEL_TAG, XmlAttribute(), chunk);
        setModel(ModelReaderWriter::loadFrom(modelChunk, xmlParser));

        std::shared_ptr<XmlChunk> physicsChunk = xmlParser.getUniqueChunk(false, PHYSICS_TAG, XmlAttribute(), chunk);
        if (physicsChunk != nullptr) {
            std::string rigidBodyId = this->name;
            const Transform<float>& modelTransform = this->model->getTransform();

            setupInteractiveBody(RigidBodyReaderWriter::loadFrom(physicsChunk, rigidBodyId, modelTransform, xmlParser));
        }
    }

    void SceneObject::writeOn(const std::shared_ptr<XmlChunk>& chunk, XmlWriter& xmlWriter) const {
        chunk->setAttribute(XmlAttribute(NAME_ATTR, this->name));

        std::shared_ptr<XmlChunk> modelChunk = xmlWriter.createChunk(MODEL_TAG, XmlAttribute(), chunk);
        ModelReaderWriter::writeOn(modelChunk, model, xmlWriter);

        if (rigidBody) {
            std::shared_ptr<XmlChunk> physicsChunk = xmlWriter.createChunk(PHYSICS_TAG, XmlAttribute(), chunk);
            RigidBodyReaderWriter::writeOn(physicsChunk, rigidBody, xmlWriter);
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

    void SceneObject::moveTo(const Transform<float>& newTransform) {
        model->setPosition(newTransform.getPosition());
        model->setOrientation(newTransform.getOrientation());
        if (aiObject) {
            aiObject->updateTransform(newTransform.getPosition(), newTransform.getOrientation());
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
