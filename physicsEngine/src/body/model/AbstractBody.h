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

    class AbstractBody {
        public:
            AbstractBody(std::string , Transform<float> , std::shared_ptr<const CollisionShape3D> );
            AbstractBody(const AbstractBody&);
            virtual ~AbstractBody() = default;

            void setIsNew(bool);
            bool isNew() const;

            void markAsDeleted();
            bool isDeleted() const;

            void setNeedFullRefresh(bool);
            bool needFullRefresh() const;

            virtual AbstractWorkBody *createWorkBody() const = 0;
            void setWorkBody(AbstractWorkBody *);
            AbstractWorkBody *getWorkBody() const;

            virtual void updateTo(AbstractWorkBody *);
            virtual bool applyFrom(const AbstractWorkBody*);

            void setTransform(const Transform<float>&);
            Transform<float> getTransform() const;
            bool isManuallyMovedAndResetFlag();

            void setShape(const std::shared_ptr<const CollisionShape3D>&);
            std::shared_ptr<const CollisionShape3D> getOriginalShape() const;
            std::shared_ptr<const CollisionShape3D> getScaledShape() const;

            void setId(const std::string&);
            const std::string &getId() const;

            void setRestitution(float);
            float getRestitution() const;
            void setFriction(float);
            float getFriction() const;
            void setRollingFriction(float);
            float getRollingFriction() const;

            float getCcdMotionThreshold() const;
            void setCcdMotionThreshold(float);

            bool isStatic() const;
            bool isActive() const;

        protected:
            void initialize(float, float, float);
            virtual void refreshScaledShape();
            Vector3<float> computeScaledShapeLocalInertia(float) const;

            void setIsStatic(bool);

            //mutex for attributes modifiable from external
            mutable std::mutex bodyMutex;

        private:
            //technical data
            const float ccdMotionThresholdFactor;
            std::atomic_bool bIsNew;
            std::atomic_bool bIsDeleted;
            std::atomic_bool bNeedFullRefresh;
            AbstractWorkBody *workBody;

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
            std::atomic_bool bIsStatic;
            std::atomic_bool bIsActive;
    };

}

#endif
