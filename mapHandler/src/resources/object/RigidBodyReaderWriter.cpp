#include "RigidBodyReaderWriter.h"
#include <resources/object/bodyshape/CollisionShapeReaderWriter.h>
#include <resources/object/bodyshape/CollisionShapeReaderWriterRetriever.h>

namespace urchin {

    RigidBody* RigidBodyReaderWriter::loadFrom(const DataChunk* physicsChunk, const std::string& id,
            const Transform<float>& modelTransform, const DataParser& dataParser) {
        auto shapeChunk = dataParser.getUniqueChunk(true, SHAPE_TAG, UdaAttribute(), physicsChunk);
        std::unique_ptr<CollisionShapeReaderWriter> shapeReaderWriter = CollisionShapeReaderWriterRetriever::retrieveShapeReaderWriter(shapeChunk);
        auto bodyShape = std::unique_ptr<CollisionShape3D>(shapeReaderWriter->loadFrom(shapeChunk, dataParser));

        auto* rigidBody = new RigidBody(id, PhysicsTransform(modelTransform.getPosition(), modelTransform.getOrientation()), std::move(bodyShape));
        loadBodyPropertiesOn(rigidBody, physicsChunk, dataParser);

        return rigidBody;
    }

    void RigidBodyReaderWriter::writeOn(DataChunk& physicsChunk, const RigidBody* rigidBody, UdaWriter& udaWriter) {
        auto& shapeChunk = udaWriter.createChunk(SHAPE_TAG, UdaAttribute(), &physicsChunk);
        std::unique_ptr<CollisionShapeReaderWriter> shapeReaderWriter = CollisionShapeReaderWriterRetriever::retrieveShapeReaderWriter(rigidBody->getShape());

        shapeReaderWriter->writeOn(shapeChunk, rigidBody->getShape(), udaWriter);
        writeBodyPropertiesOn(physicsChunk, rigidBody, udaWriter);
    }

    void RigidBodyReaderWriter::loadBodyPropertiesOn(RigidBody* rigidBody, const DataChunk* physicsChunk, const DataParser& dataParser) {
        auto massChunk = dataParser.getUniqueChunk(true, MASS_TAG, UdaAttribute(), physicsChunk);
        float bodyMass = massChunk->getFloatValue();
        rigidBody->setMass(bodyMass);

        auto restitutionChunk = dataParser.getUniqueChunk(true, RESTITUTION_TAG, UdaAttribute(), physicsChunk);
        rigidBody->setRestitution(restitutionChunk->getFloatValue());

        auto frictionChunk = dataParser.getUniqueChunk(true, FRICTION_TAG, UdaAttribute(), physicsChunk);
        rigidBody->setFriction(frictionChunk->getFloatValue());

        auto rollingFrictionChunk = dataParser.getUniqueChunk(true, ROLLING_FRICTION_TAG, UdaAttribute(), physicsChunk);
        rigidBody->setRollingFriction(rollingFrictionChunk->getFloatValue());

        auto linearDampingChunk = dataParser.getUniqueChunk(true, LINEAR_DAMPING_TAG, UdaAttribute(), physicsChunk);
        auto angularDampingChunk = dataParser.getUniqueChunk(true, ANGULAR_DAMPING_TAG, UdaAttribute(), physicsChunk);
        rigidBody->setDamping(linearDampingChunk->getFloatValue(), angularDampingChunk->getFloatValue());

        auto linearFactorChunk = dataParser.getUniqueChunk(true, LINEAR_FACTOR_TAG, UdaAttribute(), physicsChunk);
        rigidBody->setLinearFactor(linearFactorChunk->getVector3Value());

        auto angularFactorChunk = dataParser.getUniqueChunk(true, ANGULAR_FACTOR_TAG, UdaAttribute(), physicsChunk);
        rigidBody->setAngularFactor(angularFactorChunk->getVector3Value());
    }

    void RigidBodyReaderWriter::writeBodyPropertiesOn(DataChunk& physicsChunk, const RigidBody* rigidBody, UdaWriter& udaWriter) {
        auto& massChunk = udaWriter.createChunk(MASS_TAG, UdaAttribute(), &physicsChunk);
        massChunk.setFloatValue(rigidBody->getMass());

        auto& restitutionChunk = udaWriter.createChunk(RESTITUTION_TAG, UdaAttribute(), &physicsChunk);
        restitutionChunk.setFloatValue(rigidBody->getRestitution());

        auto& frictionChunk = udaWriter.createChunk(FRICTION_TAG, UdaAttribute(), &physicsChunk);
        frictionChunk.setFloatValue(rigidBody->getFriction());

        auto& rollingFrictionChunk = udaWriter.createChunk(ROLLING_FRICTION_TAG, UdaAttribute(), &physicsChunk);
        rollingFrictionChunk.setFloatValue(rigidBody->getRollingFriction());

        auto& linearDampingChunk = udaWriter.createChunk(LINEAR_DAMPING_TAG, UdaAttribute(), &physicsChunk);
        linearDampingChunk.setFloatValue(rigidBody->getLinearDamping());

        auto& angularDampingChunk = udaWriter.createChunk(ANGULAR_DAMPING_TAG, UdaAttribute(), &physicsChunk);
        angularDampingChunk.setFloatValue(rigidBody->getAngularDamping());

        auto& linearFactorChunk = udaWriter.createChunk(LINEAR_FACTOR_TAG, UdaAttribute(), &physicsChunk);
        linearFactorChunk.setVector3Value(rigidBody->getLinearFactor());

        auto& angularFactorChunk = udaWriter.createChunk(ANGULAR_FACTOR_TAG, UdaAttribute(), &physicsChunk);
        angularFactorChunk.setVector3Value(rigidBody->getAngularFactor());
    }
}
