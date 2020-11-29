#ifndef URCHINENGINE_CONTINUOUSCOLLISIONRESULT_H
#define URCHINENGINE_CONTINUOUSCOLLISIONRESULT_H

#include <set>
#include <memory>
#include "UrchinCommon.h"

#include "body/work/AbstractWorkBody.h"
#include "collision/narrowphase/algorithm/utils/AlgorithmResult.h"
#include "collision/narrowphase/algorithm/utils/AlgorithmResultDeleter.h"

namespace urchin {

    template<class T> class ContinuousCollisionResult : public AlgorithmResult {
        public:
            ContinuousCollisionResult(AbstractWorkBody*, const Vector3<T>&, const Point3<T>&, T);
            ContinuousCollisionResult(const ContinuousCollisionResult&);

            AbstractWorkBody *getBody2() const;

            const Vector3<T> &getNormalFromObject2() const;
            const Point3<T> &getHitPointOnObject2() const;
            T getTimeToHit() const;

        private:
            AbstractWorkBody *body2;

            Vector3<T> normalFromObject2;
            Point3<T> hitPointOnObject2;
            T timeToHit;
    };

    template<class T> struct ContinuousCollisionResultComparator {
        bool operator()(const std::unique_ptr<ContinuousCollisionResult<T>, AlgorithmResultDeleter>&, const std::unique_ptr<ContinuousCollisionResult<T>, AlgorithmResultDeleter>&) const;
    };

    typedef std::set<std::unique_ptr<ContinuousCollisionResult<float>, AlgorithmResultDeleter>, ContinuousCollisionResultComparator<float>> ccd_set;

}

#endif
