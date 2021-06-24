#pragma once

#include <mutex>

#include <shape/CollisionShape3D.h>
#include <body/model/GhostBody.h>

namespace urchin {

    class PhysicsCharacter {
        public:
            PhysicsCharacter(std::string, float, std::shared_ptr<const CollisionShape3D>, const PhysicsTransform&);

            const std::string& getName() const;

            float getMass() const;

            const std::shared_ptr<const CollisionShape3D>& getShape() const;

            void updateTransform(const PhysicsTransform&);
            const PhysicsTransform& getTransform() const;

        private:
            std::string name;
            float mass;
            std::shared_ptr<const CollisionShape3D> shape;
            PhysicsTransform transform;
    };

}
