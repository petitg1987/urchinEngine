#include <resources/object/RigidBodyReaderWriter.h>
#include <resources/object/bodyshape/CollisionShapeReaderWriter.h>
#include <resources/object/bodyshape/CollisionShapeReaderWriterRetriever.h>

namespace urchin {

    std::unique_ptr<RigidBody> RigidBodyReaderWriter::load(const UdaChunk* physicsChunk, const std::string& id,
            const Transform<float>& modelTransform, const UdaParser& udaParser) {
        auto shapeChunk = udaParser.getUniqueChunk(true, SHAPE_TAG, UdaAttribute(), physicsChunk);
        std::unique_ptr<CollisionShapeReaderWriter> shapeReaderWriter = CollisionShapeReaderWriterRetriever::retrieveShapeReaderWriter(shapeChunk);
        auto bodyShape = std::unique_ptr<CollisionShape3D>(shapeReaderWriter->load(shapeChunk, udaParser));

        auto rigidBody = std::make_unique<RigidBody>(id, PhysicsTransform(modelTransform.getPosition(), modelTransform.getOrientation()), std::move(bodyShape));
        loadBodyProperties(*rigidBody, physicsChunk, udaParser);

        return rigidBody;
    }

    void RigidBodyReaderWriter::write(UdaChunk& physicsChunk, const RigidBody& rigidBody, UdaWriter& udaWriter) {
        auto& shapeChunk = udaWriter.createChunk(SHAPE_TAG, UdaAttribute(), &physicsChunk);
        std::unique_ptr<CollisionShapeReaderWriter> shapeReaderWriter = CollisionShapeReaderWriterRetriever::retrieveShapeReaderWriter(rigidBody.getShape());

        shapeReaderWriter->write(shapeChunk, rigidBody.getShape(), udaWriter);
        writeBodyProperties(physicsChunk, rigidBody, udaWriter);
    }

    void RigidBodyReaderWriter::loadBodyProperties(RigidBody& rigidBody, const UdaChunk* physicsChunk, const UdaParser& udaParser) {
        auto massChunk = udaParser.getUniqueChunk(true, MASS_TAG, UdaAttribute(), physicsChunk);
        float bodyMass = massChunk->getFloatValue();
        rigidBody.setMass(bodyMass);

        auto restitutionChunk = udaParser.getUniqueChunk(true, RESTITUTION_TAG, UdaAttribute(), physicsChunk);
        rigidBody.setRestitution(restitutionChunk->getFloatValue());

        auto frictionChunk = udaParser.getUniqueChunk(true, FRICTION_TAG, UdaAttribute(), physicsChunk);
        rigidBody.setFriction(frictionChunk->getFloatValue());

        auto rollingFrictionChunk = udaParser.getUniqueChunk(true, ROLLING_FRICTION_TAG, UdaAttribute(), physicsChunk);
        rigidBody.setRollingFriction(rollingFrictionChunk->getFloatValue());

        auto linearDampingChunk = udaParser.getUniqueChunk(true, LINEAR_DAMPING_TAG, UdaAttribute(), physicsChunk);
        auto angularDampingChunk = udaParser.getUniqueChunk(true, ANGULAR_DAMPING_TAG, UdaAttribute(), physicsChunk);
        rigidBody.setDamping(linearDampingChunk->getFloatValue(), angularDampingChunk->getFloatValue());

        auto linearFactorChunk = udaParser.getUniqueChunk(true, LINEAR_FACTOR_TAG, UdaAttribute(), physicsChunk);
        rigidBody.setLinearFactor(linearFactorChunk->getVector3Value());

        auto angularFactorChunk = udaParser.getUniqueChunk(true, ANGULAR_FACTOR_TAG, UdaAttribute(), physicsChunk);
        rigidBody.setAngularFactor(angularFactorChunk->getVector3Value());
    }

    void RigidBodyReaderWriter::writeBodyProperties(UdaChunk& physicsChunk, const RigidBody& rigidBody, UdaWriter& udaWriter) {
        auto& massChunk = udaWriter.createChunk(MASS_TAG, UdaAttribute(), &physicsChunk);
        massChunk.setFloatValue(rigidBody.getMass());

        auto& restitutionChunk = udaWriter.createChunk(RESTITUTION_TAG, UdaAttribute(), &physicsChunk);
        restitutionChunk.setFloatValue(rigidBody.getRestitution());

        auto& frictionChunk = udaWriter.createChunk(FRICTION_TAG, UdaAttribute(), &physicsChunk);
        frictionChunk.setFloatValue(rigidBody.getFriction());

        auto& rollingFrictionChunk = udaWriter.createChunk(ROLLING_FRICTION_TAG, UdaAttribute(), &physicsChunk);
        rollingFrictionChunk.setFloatValue(rigidBody.getRollingFriction());

        auto& linearDampingChunk = udaWriter.createChunk(LINEAR_DAMPING_TAG, UdaAttribute(), &physicsChunk);
        linearDampingChunk.setFloatValue(rigidBody.getLinearDamping());

        auto& angularDampingChunk = udaWriter.createChunk(ANGULAR_DAMPING_TAG, UdaAttribute(), &physicsChunk);
        angularDampingChunk.setFloatValue(rigidBody.getAngularDamping());

        auto& linearFactorChunk = udaWriter.createChunk(LINEAR_FACTOR_TAG, UdaAttribute(), &physicsChunk);
        linearFactorChunk.setVector3Value(rigidBody.getLinearFactor());

        auto& angularFactorChunk = udaWriter.createChunk(ANGULAR_FACTOR_TAG, UdaAttribute(), &physicsChunk);
        angularFactorChunk.setVector3Value(rigidBody.getAngularFactor());
    }
}
