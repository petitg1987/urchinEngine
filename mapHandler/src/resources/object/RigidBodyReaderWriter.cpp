#include "RigidBodyReaderWriter.h"
#include <resources/object/bodyshape/CollisionShapeReaderWriter.h>
#include <resources/object/bodyshape/CollisionShapeReaderWriterRetriever.h>

namespace urchin {

    RigidBody* RigidBodyReaderWriter::loadFrom(const DataChunk* physicsChunk, const std::string& id,
            const Transform<float>& modelTransform, const DataParser& dataParser) {
        auto shapeChunk = dataParser.getUniqueChunk(true, SHAPE_TAG, DataAttribute(), physicsChunk);
        std::unique_ptr<CollisionShapeReaderWriter> shapeReaderWriter = CollisionShapeReaderWriterRetriever::retrieveShapeReaderWriter(shapeChunk);
        auto bodyShape = std::unique_ptr<CollisionShape3D>(shapeReaderWriter->loadFrom(shapeChunk, dataParser));

        auto* rigidBody = new RigidBody(id, PhysicsTransform(modelTransform.getPosition(), modelTransform.getOrientation()), std::move(bodyShape));
        loadBodyPropertiesOn(rigidBody, physicsChunk, dataParser);

        return rigidBody;
    }

    void RigidBodyReaderWriter::writeOn(DataChunk& physicsChunk, const RigidBody* rigidBody, DataWriter& dataWriter) {
        auto& shapeChunk = dataWriter.createChunk(SHAPE_TAG, DataAttribute(), &physicsChunk);
        std::unique_ptr<CollisionShapeReaderWriter> shapeReaderWriter = CollisionShapeReaderWriterRetriever::retrieveShapeReaderWriter(rigidBody->getShape());

        shapeReaderWriter->writeOn(shapeChunk, rigidBody->getShape(), dataWriter);
        writeBodyPropertiesOn(physicsChunk, rigidBody, dataWriter);
    }

    void RigidBodyReaderWriter::loadBodyPropertiesOn(RigidBody* rigidBody, const DataChunk* physicsChunk, const DataParser& dataParser) {
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

    void RigidBodyReaderWriter::writeBodyPropertiesOn(DataChunk& physicsChunk, const RigidBody* rigidBody, DataWriter& dataWriter) {
        auto& massChunk = dataWriter.createChunk(MASS_TAG, DataAttribute(), &physicsChunk);
        massChunk.setFloatValue(rigidBody->getMass());

        auto& restitutionChunk = dataWriter.createChunk(RESTITUTION_TAG, DataAttribute(), &physicsChunk);
        restitutionChunk.setFloatValue(rigidBody->getRestitution());

        auto& frictionChunk = dataWriter.createChunk(FRICTION_TAG, DataAttribute(), &physicsChunk);
        frictionChunk.setFloatValue(rigidBody->getFriction());

        auto& rollingFrictionChunk = dataWriter.createChunk(ROLLING_FRICTION_TAG, DataAttribute(), &physicsChunk);
        rollingFrictionChunk.setFloatValue(rigidBody->getRollingFriction());

        auto& linearDampingChunk = dataWriter.createChunk(LINEAR_DAMPING_TAG, DataAttribute(), &physicsChunk);
        linearDampingChunk.setFloatValue(rigidBody->getLinearDamping());

        auto& angularDampingChunk = dataWriter.createChunk(ANGULAR_DAMPING_TAG, DataAttribute(), &physicsChunk);
        angularDampingChunk.setFloatValue(rigidBody->getAngularDamping());

        auto& linearFactorChunk = dataWriter.createChunk(LINEAR_FACTOR_TAG, DataAttribute(), &physicsChunk);
        linearFactorChunk.setVector3Value(rigidBody->getLinearFactor());

        auto& angularFactorChunk = dataWriter.createChunk(ANGULAR_FACTOR_TAG, DataAttribute(), &physicsChunk);
        angularFactorChunk.setVector3Value(rigidBody->getAngularFactor());
    }
}
