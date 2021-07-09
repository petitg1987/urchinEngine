#include "RigidBodyReaderWriter.h"
#include <resources/object/bodyshape/CollisionShapeReaderWriter.h>
#include <resources/object/bodyshape/CollisionShapeReaderWriterRetriever.h>

namespace urchin {

    RigidBody* RigidBodyReaderWriter::loadFrom(const XmlChunk* physicsChunk, const std::string& id,
            const Transform<float>& modelTransform, const XmlParser& xmlParser) {
        auto shapeChunk = xmlParser.getUniqueChunk(true, SHAPE_TAG, XmlAttribute(), physicsChunk);
        std::shared_ptr<CollisionShapeReaderWriter> shapeReaderWriter = CollisionShapeReaderWriterRetriever::retrieveShapeReaderWriter(shapeChunk.get());
        auto bodyShape = std::unique_ptr<CollisionShape3D>(shapeReaderWriter->loadFrom(shapeChunk.get(), xmlParser));

        auto* rigidBody = new RigidBody(id, PhysicsTransform(modelTransform.getPosition(), modelTransform.getOrientation()), std::move(bodyShape));
        loadBodyPropertiesOn(rigidBody, physicsChunk, xmlParser);

        return rigidBody;
    }

    void RigidBodyReaderWriter::writeOn(XmlChunk* physicsChunk, const RigidBody* rigidBody, XmlWriter& xmlWriter) {
        auto shapeChunk = xmlWriter.createChunk(SHAPE_TAG, XmlAttribute(), physicsChunk);
        std::shared_ptr<CollisionShapeReaderWriter> shapeReaderWriter = CollisionShapeReaderWriterRetriever::retrieveShapeReaderWriter(rigidBody->getShape());

        shapeReaderWriter->writeOn(shapeChunk.get(), rigidBody->getShape(), xmlWriter);
        writeBodyPropertiesOn(physicsChunk, rigidBody, xmlWriter);
    }

    void RigidBodyReaderWriter::loadBodyPropertiesOn(RigidBody* rigidBody, const XmlChunk* physicsChunk, const XmlParser& xmlParser) {
        auto massChunk = xmlParser.getUniqueChunk(true, MASS_TAG, XmlAttribute(), physicsChunk);
        float bodyMass = massChunk->getFloatValue();
        rigidBody->setMass(bodyMass);

        auto restitutionChunk = xmlParser.getUniqueChunk(true, RESTITUTION_TAG, XmlAttribute(), physicsChunk);
        rigidBody->setRestitution(restitutionChunk->getFloatValue());

        auto frictionChunk = xmlParser.getUniqueChunk(true, FRICTION_TAG, XmlAttribute(), physicsChunk);
        rigidBody->setFriction(frictionChunk->getFloatValue());

        auto rollingFrictionChunk = xmlParser.getUniqueChunk(true, ROLLING_FRICTION_TAG, XmlAttribute(), physicsChunk);
        rigidBody->setRollingFriction(rollingFrictionChunk->getFloatValue());

        auto linearDampingChunk = xmlParser.getUniqueChunk(true, LINEAR_DAMPING_TAG, XmlAttribute(), physicsChunk);
        auto angularDampingChunk = xmlParser.getUniqueChunk(true, ANGULAR_DAMPING_TAG, XmlAttribute(), physicsChunk);
        rigidBody->setDamping(linearDampingChunk->getFloatValue(), angularDampingChunk->getFloatValue());

        auto linearFactorChunk = xmlParser.getUniqueChunk(true, LINEAR_FACTOR_TAG, XmlAttribute(), physicsChunk);
        rigidBody->setLinearFactor(linearFactorChunk->getVector3Value());

        auto angularFactorChunk = xmlParser.getUniqueChunk(true, ANGULAR_FACTOR_TAG, XmlAttribute(), physicsChunk);
        rigidBody->setAngularFactor(angularFactorChunk->getVector3Value());
    }

    void RigidBodyReaderWriter::writeBodyPropertiesOn(const XmlChunk* physicsChunk, const RigidBody* rigidBody, XmlWriter& xmlWriter) {
        auto massChunk = xmlWriter.createChunk(MASS_TAG, XmlAttribute(), physicsChunk);
        massChunk->setFloatValue(rigidBody->getMass());

        auto restitutionChunk = xmlWriter.createChunk(RESTITUTION_TAG, XmlAttribute(), physicsChunk);
        restitutionChunk->setFloatValue(rigidBody->getRestitution());

        auto frictionChunk = xmlWriter.createChunk(FRICTION_TAG, XmlAttribute(), physicsChunk);
        frictionChunk->setFloatValue(rigidBody->getFriction());

        auto rollingFrictionChunk = xmlWriter.createChunk(ROLLING_FRICTION_TAG, XmlAttribute(), physicsChunk);
        rollingFrictionChunk->setFloatValue(rigidBody->getRollingFriction());

        auto linearDampingChunk = xmlWriter.createChunk(LINEAR_DAMPING_TAG, XmlAttribute(), physicsChunk);
        linearDampingChunk->setFloatValue(rigidBody->getLinearDamping());

        auto angularDampingChunk = xmlWriter.createChunk(ANGULAR_DAMPING_TAG, XmlAttribute(), physicsChunk);
        angularDampingChunk->setFloatValue(rigidBody->getAngularDamping());

        auto linearFactorChunk = xmlWriter.createChunk(LINEAR_FACTOR_TAG, XmlAttribute(), physicsChunk);
        linearFactorChunk->setVector3Value(rigidBody->getLinearFactor());

        auto angularFactorChunk = xmlWriter.createChunk(ANGULAR_FACTOR_TAG, XmlAttribute(), physicsChunk);
        angularFactorChunk->setVector3Value(rigidBody->getAngularFactor());
    }
}
