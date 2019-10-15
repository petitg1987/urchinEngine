#include "RigidBodyReaderWriter.h"
#include "bodyshape/CollisionShapeReaderWriter.h"
#include "bodyshape/CollisionShapeReaderWriterRetriever.h"

namespace urchin
{

	RigidBody *RigidBodyReaderWriter::loadFrom(const std::shared_ptr<XmlChunk> &physicsChunk, const std::string &id,
			const Transform<float> &modelTransform, const XmlParser &xmlParser) const
	{
		std::shared_ptr<XmlChunk> shapeChunk = xmlParser.getUniqueChunk(true, SHAPE_TAG, XmlAttribute(), physicsChunk);
		std::shared_ptr<CollisionShapeReaderWriter> shapeReaderWriter = CollisionShapeReaderWriterRetriever::retrieveShapeReaderWriter(shapeChunk);
		CollisionShape3D *bodyShape = shapeReaderWriter->loadFrom(shapeChunk, xmlParser);

		auto *rigidBody = new RigidBody(id, modelTransform, std::shared_ptr<const CollisionShape3D>(bodyShape));
		loadBodyPropertiesOn(rigidBody, physicsChunk, xmlParser);

		return rigidBody;
	}

	void RigidBodyReaderWriter::writeOn(const std::shared_ptr<XmlChunk> &physicsChunk, const RigidBody *rigidBody, XmlWriter &xmlWriter) const
	{
		std::shared_ptr<XmlChunk> shapeChunk = xmlWriter.createChunk(SHAPE_TAG, XmlAttribute(), physicsChunk);
		std::shared_ptr<CollisionShapeReaderWriter> shapeReaderWriter = CollisionShapeReaderWriterRetriever::retrieveShapeReaderWriter(rigidBody->getOriginalShape().get());

		shapeReaderWriter->writeOn(shapeChunk, rigidBody->getOriginalShape().get(), xmlWriter);
		writeBodyPropertiesOn(physicsChunk, rigidBody, xmlWriter);
	}

	void RigidBodyReaderWriter::loadBodyPropertiesOn(RigidBody *rigidBody, const std::shared_ptr<XmlChunk> &physicsChunk, const XmlParser &xmlParser) const
	{
		std::shared_ptr<XmlChunk> massChunk = xmlParser.getUniqueChunk(true, MASS_TAG, XmlAttribute(), physicsChunk);
		float bodyMass = massChunk->getFloatValue();
		rigidBody->setMass(bodyMass);

		std::shared_ptr<XmlChunk> restitutionChunk = xmlParser.getUniqueChunk(true, RESTITUTION_TAG, XmlAttribute(), physicsChunk);
		rigidBody->setRestitution(restitutionChunk->getFloatValue());

		std::shared_ptr<XmlChunk> frictionChunk = xmlParser.getUniqueChunk(true, FRICTION_TAG, XmlAttribute(), physicsChunk);
		rigidBody->setFriction(frictionChunk->getFloatValue());

		std::shared_ptr<XmlChunk> rollingFrictionChunk = xmlParser.getUniqueChunk(true, ROLLING_FRICTION_TAG, XmlAttribute(), physicsChunk);
		rigidBody->setRollingFriction(rollingFrictionChunk->getFloatValue());

		std::shared_ptr<XmlChunk> linearDampingChunk = xmlParser.getUniqueChunk(true, LINEAR_DAMPING_TAG, XmlAttribute(), physicsChunk);
		std::shared_ptr<XmlChunk> angularDampingChunk = xmlParser.getUniqueChunk(true, ANGULAR_DAMPING_TAG, XmlAttribute(), physicsChunk);
		rigidBody->setDamping(linearDampingChunk->getFloatValue(), angularDampingChunk->getFloatValue());

		std::shared_ptr<XmlChunk> linearFactorChunk = xmlParser.getUniqueChunk(true, LINEAR_FACTOR_TAG, XmlAttribute(), physicsChunk);
		rigidBody->setLinearFactor(linearFactorChunk->getVector3Value());

		std::shared_ptr<XmlChunk> angularFactorChunk = xmlParser.getUniqueChunk(true, ANGULAR_FACTOR_TAG, XmlAttribute(), physicsChunk);
		rigidBody->setAngularFactor(angularFactorChunk->getVector3Value());
	}

	void RigidBodyReaderWriter::writeBodyPropertiesOn(const std::shared_ptr<XmlChunk> &physicsChunk, const RigidBody *rigidBody, XmlWriter &xmlWriter) const
	{
		std::shared_ptr<XmlChunk> massChunk = xmlWriter.createChunk(MASS_TAG, XmlAttribute(), physicsChunk);
		massChunk->setFloatValue(rigidBody->getMass());

		std::shared_ptr<XmlChunk> restitutionChunk = xmlWriter.createChunk(RESTITUTION_TAG, XmlAttribute(), physicsChunk);
		restitutionChunk->setFloatValue(rigidBody->getRestitution());

		std::shared_ptr<XmlChunk> frictionChunk = xmlWriter.createChunk(FRICTION_TAG, XmlAttribute(), physicsChunk);
		frictionChunk->setFloatValue(rigidBody->getFriction());

		std::shared_ptr<XmlChunk> rollingFrictionChunk = xmlWriter.createChunk(ROLLING_FRICTION_TAG, XmlAttribute(), physicsChunk);
		rollingFrictionChunk->setFloatValue(rigidBody->getRollingFriction());

		std::shared_ptr<XmlChunk> linearDampingChunk = xmlWriter.createChunk(LINEAR_DAMPING_TAG, XmlAttribute(), physicsChunk);
		linearDampingChunk->setFloatValue(rigidBody->getLinearDamping());

		std::shared_ptr<XmlChunk> angularDampingChunk = xmlWriter.createChunk(ANGULAR_DAMPING_TAG, XmlAttribute(), physicsChunk);
		angularDampingChunk->setFloatValue(rigidBody->getAngularDamping());

		std::shared_ptr<XmlChunk> linearFactorChunk = xmlWriter.createChunk(LINEAR_FACTOR_TAG, XmlAttribute(), physicsChunk);
		linearFactorChunk->setVector3Value(rigidBody->getLinearFactor());

		std::shared_ptr<XmlChunk> angularFactorChunk = xmlWriter.createChunk(ANGULAR_FACTOR_TAG, XmlAttribute(), physicsChunk);
		angularFactorChunk->setVector3Value(rigidBody->getAngularFactor());
	}
}
