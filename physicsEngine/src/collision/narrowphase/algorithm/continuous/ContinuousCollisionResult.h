#pragma once

#include <memory>
#include <UrchinCommon.h>

#include <body/model/AbstractBody.h>

namespace urchin {

    template<class T> class ContinuousCollisionResult {
        public:
            static ContinuousCollisionResult<T> newNoResult();
            static ContinuousCollisionResult<T> newResult(std::shared_ptr<AbstractBody>, std::size_t, const Vector3<T>&, const Point3<T>&, T);

            bool hasResult() const;

            AbstractBody& getBody2() const;
            std::size_t getShapeIndex() const;

            const Vector3<T>& getNormalFromObject2() const;
            const Point3<T>& getHitPointOnObject2() const;
            T getTimeToHit() const;

        private:
            ContinuousCollisionResult(std::shared_ptr<AbstractBody>, std::size_t, const Vector3<T>&, const Point3<T>&, T);

            std::shared_ptr<AbstractBody> body2; //own the body as this attribute can be used outside the physics thread where body could be removed
            std::size_t shapeIndex;

            Vector3<T> normalFromObject2;
            Point3<T> hitPointOnObject2;
            T timeToHit;
    };

    template<class T> struct ContinuousCollisionResultComparator {
        bool operator()(const ContinuousCollisionResult<T>&, const ContinuousCollisionResult<T>&) const;
    };

}
