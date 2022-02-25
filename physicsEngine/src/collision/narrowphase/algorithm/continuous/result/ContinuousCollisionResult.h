#pragma once

#include <set>
#include <memory>
#include <UrchinCommon.h>

#include <body/model/AbstractBody.h>
#include <collision/narrowphase/algorithm/util/AlgorithmResult.h>
#include <collision/narrowphase/algorithm/util/AlgorithmResultDeleter.h>

namespace urchin {

    template<class T> class ContinuousCollisionResult : public AlgorithmResult {
        public:
            ContinuousCollisionResult(std::shared_ptr<AbstractBody>, const Vector3<T>&, const Point3<T>&, T);

            AbstractBody& getBody2() const;

            const Vector3<T>& getNormalFromObject2() const;
            const Point3<T>& getHitPointOnObject2() const;
            T getTimeToHit() const;

        private:
            std::shared_ptr<AbstractBody> body2; //own the body as this attribute can be used outside the physics thread where body could be removed

            Vector3<T> normalFromObject2;
            Point3<T> hitPointOnObject2;
            T timeToHit;
    };

    template<class T> struct ContinuousCollisionResultComparator {
        bool operator()(const std::unique_ptr<ContinuousCollisionResult<T>, AlgorithmResultDeleter>&, const std::unique_ptr<ContinuousCollisionResult<T>, AlgorithmResultDeleter>&) const;
    };

    using ccd_set = std::set<std::unique_ptr<ContinuousCollisionResult<float>, AlgorithmResultDeleter>, ContinuousCollisionResultComparator<float>>;

}
