#ifndef URCHINENGINE_PHYSICSCHARACTER_H
#define URCHINENGINE_PHYSICSCHARACTER_H

#include <mutex>

#include <shape/CollisionShape3D.h>
#include <body/model/GhostBody.h>

namespace urchin {

    class PhysicsCharacter {
        public:
            PhysicsCharacter(const std::string&, float, const std::shared_ptr<const CollisionShape3D>&, const PhysicsTransform&);
            PhysicsCharacter(std::string , float, std::shared_ptr<const CollisionShape3D> , const PhysicsTransform&, float, float);

            const std::string& getName() const;

            float getMass() const;

            const std::shared_ptr<const CollisionShape3D>& getShape() const;

            void updateTransform(const PhysicsTransform&);
            const PhysicsTransform& getTransform() const;

            float getJumpSpeed() const;

            float getMaxSlopeInRadian() const;
            float getMaxSlopeInPercentage() const;

        private:
            std::string name;
            float mass;
            std::shared_ptr<const CollisionShape3D> shape;
            PhysicsTransform transform;

            float jumpSpeed;
            float maxSlopeInRadian, maxSlopeInPercentage;

            mutable std::mutex characterMutex;
    };

}

#endif
