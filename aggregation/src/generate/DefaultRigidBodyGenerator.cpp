#include <generate/DefaultRigidBodyGenerator.h>
#include <generate/DefaultBodyShapeGenerator.h>

namespace urchin {

    DefaultRigidBodyGenerator::DefaultRigidBodyGenerator(const ObjectEntity& objectEntity) :
            objectEntity(objectEntity) {

    }

    std::unique_ptr<RigidBody> DefaultRigidBodyGenerator::generate(CollisionShape3D::ShapeType shapeType) {
        Transform<float> modelTransform = objectEntity.getModel()->getTransform();
        PhysicsTransform physicsTransform(modelTransform.getPosition(), modelTransform.getOrientation());
        std::unique_ptr<const CollisionShape3D> bodyShape = DefaultBodyShapeGenerator(objectEntity).generate(shapeType);

        return std::make_unique<RigidBody>(objectEntity.getName(), physicsTransform, std::move(bodyShape));
    }

}
