#include "RigidBodyReaderWriter.h"
#include <resources/object/bodyshape/CollisionShapeReaderWriter.h>
#include <resources/object/bodyshape/CollisionShapeReaderWriterRetriever.h>

namespace urchin {

    RigidBody* RigidBodyReaderWriter::loadFrom(const XmlChunk* physicsChunk, const std::string& id,
            const Transform<float>& modelTransform, const DataParser& dataParser) {
        auto shapeChunk = dataParser.getUniqueChunk(true, SHAPE_TAG, DataAttribute(), physicsChunk);
        std::shared_ptr<CollisionShapeReaderWriter> shapeReaderWriter = CollisionShapeReaderWriterRetriever::retrieveShapeReaderWriter(shapeChunk.get());
        auto bodyShape = std::unique_ptr<CollisionShape3D>(shapeReaderWriter->loadFrom(shapeChunk.get(), dataParser));

        auto* rigidBody = new RigidBody(id, PhysicsTransform(modelTransform.getPosition(), modelTransform.getOrientation()), std::move(bodyShape));
        loadBodyPropertiesOn(rigidBody, physicsChunk, dataParser);

        return rigidBody;
    }

    void RigidBodyReaderWriter::writeOn(XmlChunk* physicsChunk, const RigidBody* rigidBody, XmlWriter& xmlWriter) {
        auto shapeChunk = xmlWriter.createChunk(SHAPE_TAG, DataAttribute(), physicsChunk);
        std::shared_ptr<CollisionShapeReaderWriter> shapeReaderWriter = CollisionShapeReaderWriterRetriever::retrieveShapeReaderWriter(rigidBody->getShape());

        shapeReaderWriter->writeOn(shapeChunk.get(), rigidBody->getShape(), xmlWriter);
        writeBodyPropertiesOn(physicsChunk, rigidBody, xmlWriter);
    }

    void RigidBodyReaderWriter::loadBodyPropertiesOn(RigidBody* rigidBody, const XmlChunk* physicsChunk, const DataParser& dataParser) {
        auto massChunk = dataParser.getUniqueChunk(true, MASS_TAG, DataAttribute(), physicsChunk);
        float bodyMass = massChunk->getFloatValue();
        rigidBody->setMass(bodyMass);

        auto restitutionChunk = dataParser.getUniqueChunk(true, RESTITUTION_TAG, DataAttribute(), physicsChunk);
        rigidBody->setRestitution(restitutionChunk->getFloatValue());

        auto frictionChunk = dataParser.getUniqueChunk(true, FRICTION_TAG, DataAttribute(), physicsChunk);
        rigidBody->setFriction(frictionChunk->getFloatValue());

        auto rollingFrictionChunk = dataParser.getUniqueChunk(true, ROLLING_FRICTION_TAG, DataAttribute(), physicsChunk);
        rigidBody->setRollingFriction(rollingFrictionChunk->getFloatValue());

        auto linearDampingChunk = dataParser.getUniqueChunk(true, LINEAR_DAMPING_TAG, DataAttribute(), physicsChunk);
        auto angularDampingChunk = dataParser.getUniqueChunk(true, ANGULAR_DAMPING_TAG, DataAttribute(), physicsChunk);
        rigidBody->setDamping(linearDampingChunk->getFloatValue(), angularDampingChunk->getFloatValue());

        auto linearFactorChunk = dataParser.getUniqueChunk(true, LINEAR_FACTOR_TAG, DataAttribute(), physicsChunk);
        rigidBody->setLinearFactor(linearFactorChunk->getVector3Value());

        auto angularFactorChunk = dataParser.getUniqueChunk(true, ANGULAR_FACTOR_TAG, DataAttribute(), physicsChunk);
        rigidBody->setAngularFactor(angularFactorChunk->getVector3Value());
    }

    void RigidBodyReaderWriter::writeBodyPropertiesOn(const XmlChunk* physicsChunk, const RigidBody* rigidBody, XmlWriter& xmlWriter) {
        auto massChunk = xmlWriter.createChunk(MASS_TAG, DataAttribute(), physicsChunk);
        massChunk->setFloatValue(rigidBody->getMass());

        auto restitutionChunk = xmlWriter.createChunk(RESTITUTION_TAG, DataAttribute(), physicsChunk);
        restitutionChunk->setFloatValue(rigidBody->getRestitution());

        auto frictionChunk = xmlWriter.createChunk(FRICTION_TAG, DataAttribute(), physicsChunk);
        frictionChunk->setFloatValue(rigidBody->getFriction());

        auto rollingFrictionChunk = xmlWriter.createChunk(ROLLING_FRICTION_TAG, DataAttribute(), physicsChunk);
        rollingFrictionChunk->setFloatValue(rigidBody->getRollingFriction());

        auto linearDampingChunk = xmlWriter.createChunk(LINEAR_DAMPING_TAG, DataAttribute(), physicsChunk);
        linearDampingChunk->setFloatValue(rigidBody->getLinearDamping());

        auto angularDampingChunk = xmlWriter.createChunk(ANGULAR_DAMPING_TAG, DataAttribute(), physicsChunk);
        angularDampingChunk->setFloatValue(rigidBody->getAngularDamping());

        auto linearFactorChunk = xmlWriter.createChunk(LINEAR_FACTOR_TAG, DataAttribute(), physicsChunk);
        linearFactorChunk->setVector3Value(rigidBody->getLinearFactor());

        auto angularFactorChunk = xmlWriter.createChunk(ANGULAR_FACTOR_TAG, DataAttribute(), physicsChunk);
        angularFactorChunk->setVector3Value(rigidBody->getAngularFactor());
    }
}
