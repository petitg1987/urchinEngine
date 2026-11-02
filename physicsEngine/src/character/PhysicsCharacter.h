#pragma once

#include "shape/CollisionShape3D.h"

namespace urchin {

    class PhysicsCharacter {
        public:
            PhysicsCharacter(std::string, float, std::unique_ptr<const CollisionShape3D>, const PhysicsTransform&, float crouchHeight = -1.0f);

            const std::string& getName() const;

            float getMass() const;

            const CollisionShape3D& getStandingShape() const;
            float getCrouchHeight() const;
            bool isCrouchEnabled() const;

            void updateTransform(const PhysicsTransform&);
            const PhysicsTransform& getTransform() const;

        private:
            std::string name;
            float mass;
            std::unique_ptr<const CollisionShape3D> standingShape;
            float crouchHeight;
            PhysicsTransform transform; //transform of the standing character (even when the character is crouched)
    };

}
