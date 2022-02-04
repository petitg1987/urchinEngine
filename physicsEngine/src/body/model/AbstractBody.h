#pragma once

#include <string>
#include <memory>
#include <atomic>
#include <mutex>
#include <thread>
#include <UrchinCommon.h>

#include <collision/bodystate/IslandElement.h>
#include <shape/CollisionShape3D.h>

namespace urchin {

    enum class BodyType {
            RIGID,
            GHOST
    };

    class PairContainer;

    class AbstractBody : public IslandElement {
        public:
            //Factor used to determine the default continuous collision detection motion threshold.
            //The threshold is this factor multiplied by the minimum half size of the body shape.
            static constexpr float CCD_MOTION_THRESHOLD_FACTOR = 0.8f;

            AbstractBody(BodyType, std::string, const PhysicsTransform&, std::unique_ptr<const CollisionShape3D>);
            AbstractBody(const AbstractBody&);
            ~AbstractBody() override = default;

            void setPhysicsThreadId(std::thread::id);

            virtual void setTransform(const PhysicsTransform&);
            PhysicsTransform getTransform() const;
            bool getManuallyMovedAndReset();

            const CollisionShape3D& getShape() const;

            BodyType getBodyType() const;
            void setId(std::string);
            const std::string& getId() const;

            void setRestitution(float);
            float getRestitution() const;
            void setFriction(float);
            float getFriction() const;
            void setRollingFriction(float);
            float getRollingFriction() const;

            void setCcdMotionThreshold(float);
            float getCcdMotionThreshold() const;

            virtual PairContainer* getPairContainer() const;

            static void disableAllBodies(bool);
            bool isStatic() const;
            void setIsActive(bool);
            bool isActive() const override;

            uint_fast32_t getObjectId() const;

        protected:
            void initialize(float, float, float);

            void setIsStatic(bool);

            //technical data
            std::thread::id physicsThreadId;

            //mutex for attributes modifiable from external
            mutable std::mutex bodyMutex;

            //body representation data
            PhysicsTransform transform;
            std::atomic_bool isManuallyMoved;

        private:
            //body description data
            BodyType bodyType;
            std::string id;
            std::unique_ptr<const CollisionShape3D> shape;
            float restitution;
            float friction;
            float rollingFriction;
            float ccdMotionThreshold;

            //state flags
            static bool bDisableAllBodies;
            std::atomic_bool bIsStatic;
            std::atomic_bool bIsActive;

            //technical object id
            static uint_fast32_t nextObjectId;
            uint_fast32_t objectId;
    };

}
