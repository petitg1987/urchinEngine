#pragma once

#include <model/ObjectEntity.h>

namespace urchin {

    class DefaultRigidBodyGenerator {
        public:
            explicit DefaultRigidBodyGenerator(const ObjectEntity&);

            std::unique_ptr<RigidBody> generate(CollisionShape3D::ShapeType);

        private:
            const ObjectEntity& objectEntity;
    };

}