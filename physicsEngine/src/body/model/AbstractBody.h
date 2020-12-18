#ifndef URCHINENGINE_ABSTRACTBODY_H
#define URCHINENGINE_ABSTRACTBODY_H

#include <string>
#include <memory>
#include <atomic>
#include <mutex>
#include "UrchinCommon.h"

#include "collision/island/IslandElement.h"
#include "shape/CollisionShape3D.h"

namespace urchin {

    class PairContainer;

    class AbstractBody : public IslandElement {
        public:
            AbstractBody(std::string, const PhysicsTransform&, std::shared_ptr<const CollisionShape3D>);
            AbstractBody(const AbstractBody&);
            ~AbstractBody() override = default;

            void setNeedFullRefresh(bool);
            bool needFullRefresh() const;

            virtual void setTransform(const PhysicsTransform&); //TODO [URGENT] required full refresh and must be called at right moment to not erase value
            PhysicsTransform getTransform() const;
            virtual void setPosition(const Point3<float>&); //TODO internal only (remove method ?)
            Point3<float> getPosition() const;
            virtual void setOrientation(const Quaternion<float>&); //TODO internal only (remove method ?)
            Quaternion<float> getOrientation() const;
            bool isManuallyMovedAndResetFlag(); //TODO [URGENT] review this method

            const std::shared_ptr<const CollisionShape3D>& getShape() const;

            void setId(const std::string&);
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
            virtual void setIsStatic(bool);
            bool isStatic() const;
            void setIsActive(bool); //TODO internal only
            bool isActive() const override;
            virtual bool isGhostBody() const = 0;

            uint_fast32_t getObjectId() const;

        protected:
            void initialize(float, float, float);

            //mutex for attributes modifiable from external
            mutable std::mutex bodyMutex;

            //body representation data
            PhysicsTransform transform;
            bool isManuallyMoved;

        private:
            //technical data
            const float ccdMotionThresholdFactor;
            std::atomic_bool bNeedFullRefresh;

            //body description data
            std::string id;
            std::shared_ptr<const CollisionShape3D> shape;
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

#endif
