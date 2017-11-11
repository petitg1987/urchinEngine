#include <stdexcept>
#include <AIManager.h>

#include "SceneObject.h"
#include "resources/object/ModelReaderWriter.h"
#include "resources/object/RigidBodyReaderWriter.h"
#include "resources/object/aiobject/AIObjectBuilder.h"

namespace urchin
{

	SceneObject::SceneObject() :
			renderer3d(nullptr),
			physicsWorld(nullptr),
			aiManager(nullptr),
			model(nullptr),
			rigidBody(nullptr),
			aiObject(nullptr)
	{

	}

	SceneObject::~SceneObject()
	{
		renderer3d->removeModel(model);
		deleteRigidBody();
		deleteAIObjects();
	}

	void SceneObject::setObjectManagers(Renderer3d *renderer3d, PhysicsWorld *physicsWorld, AIManager *aiManager)
	{
		if(this->renderer3d!=nullptr)
		{
			throw std::invalid_argument("Cannot add the scene object on two different object managers.");
		}
		if(renderer3d==nullptr)
		{
			throw std::invalid_argument("Cannot specify a null renderer 3d for a scene object.");
		}

		this->renderer3d = renderer3d;
		this->physicsWorld = physicsWorld;
		this->aiManager = aiManager;

		renderer3d->addModel(model);

		if(physicsWorld!=nullptr)
		{
			physicsWorld->addBody(rigidBody);
		}

		if(aiManager!=nullptr)
		{
			aiManager->addObject(aiObject);
		}
	}

	void SceneObject::loadFrom(std::shared_ptr<XmlChunk> chunk, const XmlParser &xmlParser)
	{
		this->name = chunk->getAttributeValue(NAME_ATTR);

		std::shared_ptr<XmlChunk> modelChunk = xmlParser.getUniqueChunk(true, MODEL_TAG, XmlAttribute(), chunk);
		setModel(ModelReaderWriter().loadFrom(modelChunk, xmlParser));

		std::shared_ptr<XmlChunk> physicsChunk = xmlParser.getUniqueChunk(false, PHYSICS_TAG, XmlAttribute(), chunk);
		if(physicsChunk != nullptr)
		{
			std::string rigidBodyId = this->name;
			const Transform<float> &modelTransform = this->model->getTransform();

			setupInteractiveBody(RigidBodyReaderWriter().loadFrom(physicsChunk, rigidBodyId, modelTransform, xmlParser));
		}
	}

	void SceneObject::writeOn(std::shared_ptr<XmlChunk> chunk, XmlWriter &xmlWriter) const
	{
		chunk->setAttribute(XmlAttribute(NAME_ATTR, this->name));

		std::shared_ptr<XmlChunk> modelChunk = xmlWriter.createChunk(MODEL_TAG, XmlAttribute(), chunk);
		ModelReaderWriter().writeOn(modelChunk, model, xmlWriter);

		if(rigidBody!=nullptr)
		{
			std::shared_ptr<XmlChunk> physicsChunk = xmlWriter.createChunk(PHYSICS_TAG, XmlAttribute(), chunk);
			RigidBodyReaderWriter().writeOn(physicsChunk, rigidBody, xmlWriter);
		}
	}

	const std::string &SceneObject::getName() const
	{
		return name;
	}

	void SceneObject::setName(const std::string &name)
	{
		this->name = name;
	}

	Model *SceneObject::getModel() const
	{
		return model;
	}

	void SceneObject::setModel(Model *model)
	{
		if(model==nullptr)
		{
			throw std::invalid_argument("Cannot set a null model on scene object.");
		}

		if(renderer3d!=nullptr)
		{
			renderer3d->removeModel(this->model);
			renderer3d->addModel(model);
		}else
		{
			delete this->model;
		}

		this->model = model;
	}

	RigidBody *SceneObject::getRigidBody() const
	{
		return rigidBody;
	}

	const std::shared_ptr<AIObject> &SceneObject::getAIObject() const
	{
		return aiObject;
	}

	void SceneObject::setupInteractiveBody(RigidBody *rigidBody)
	{
		setupRigidBody(rigidBody);
		setupAIObject(rigidBody);
	}

	void SceneObject::setupRigidBody(RigidBody *rigidBody)
	{
		deleteRigidBody();

		this->rigidBody = rigidBody;
		if(physicsWorld!=nullptr)
		{
			physicsWorld->addBody(rigidBody);
		}
	}

	void SceneObject::setupAIObject(RigidBody *rigidBody)
	{
		deleteAIObjects();

		this->aiObject = AIObjectBuilder::buildAIObject(rigidBody->getId(), rigidBody->getScaledShape(), rigidBody->getTransform());
		if(aiManager!=nullptr)
		{
			aiManager->addObject(aiObject);
		}
	}

	void SceneObject::deleteRigidBody()
	{
		if(physicsWorld!=nullptr)
		{
			physicsWorld->removeBody(rigidBody);
		}else
		{
			delete rigidBody;
		}

		rigidBody = nullptr;
	}

	void SceneObject::deleteAIObjects()
	{
		if(aiManager!=nullptr)
		{
			aiManager->removeObject(aiObject);
		}
	}

}
