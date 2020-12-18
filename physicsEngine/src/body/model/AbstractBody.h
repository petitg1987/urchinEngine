#ifndef URCHINENGINE_ABSTRACTBODY_H
#define URCHINENGINE_ABSTRACTBODY_H

#include <string>
#include <memory>
#include <atomic>
#include <mutex>
#include "UrchinCommon.h"

#include "body/work/AbstractWorkBody.h"
#include "shape/CollisionShape3D.h"

namespace urchin {

    class AbstractBody : public IslandElement {
        public:
            AbstractBody(std::string, Transform<float>, std::shared_ptr<const CollisionShape3D>);
            AbstractBody(const AbstractBody&);
            ~AbstractBody() override = default;

            void setTransform(const Transform<float>&);
            Transform<float> getTransform() const; //TODO remove or keep for external user ?
            PhysicsTransform getPhysicsTransform() const;
            void setPosition(const Point3<float>&); //TODO internal only or required full refresh !
            Point3<float> getPosition() const;
            void setOrientation(const Quaternion<float>&); //TODO internal only or required full refresh !
            Quaternion<float> getOrientation() const;
            bool isManuallyMovedAndResetFlag();

            void setShape(const std::shared_ptr<const CollisionShape3D>&); //TODO remove it or only if paused + REMOVE_BODY & ADD_BODY event !
            const std::shared_ptr<const CollisionShape3D>& getOriginalShape() const;
            const std::shared_ptr<const CollisionShape3D>& getScaledShape() const; //TODO remove method duplication with getShape()
            const CollisionShape3D* getShape() const;

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
            virtual void refreshScaledShape();
            Vector3<float> computeScaledShapeLocalInertia(float) const;

            //mutex for attributes modifiable from external
            mutable std::mutex bodyMutex;

        private:
            //technical data
            const float ccdMotionThresholdFactor;

            //body representation data
            Transform<float> transform;
            bool isManuallyMoved;

            //body description data
            std::string id;
            std::shared_ptr<const CollisionShape3D> originalShape;
            std::shared_ptr<const CollisionShape3D> scaledShape;
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
