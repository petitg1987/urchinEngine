#ifndef URCHINENGINE_ALGORITHMRESULTALLOCATOR_H
#define URCHINENGINE_ALGORITHMRESULTALLOCATOR_H

#include "UrchinCommon.h"

#include "utils/pool/SyncFixedSizePool.h"
#include "collision/narrowphase/algorithm/utils/AlgorithmResult.h"
#include "collision/narrowphase/algorithm/utils/AlgorithmResultDeleter.h"
#include "collision/narrowphase/algorithm/gjk/result/GJKResultCollide.h"
#include "collision/narrowphase/algorithm/gjk/result/GJKResultInvalid.h"
#include "collision/narrowphase/algorithm/gjk/result/GJKResultNoCollide.h"
#include "collision/narrowphase/algorithm/epa/result/EPAResultCollide.h"
#include "collision/narrowphase/algorithm/epa/result/EPAResultInvalid.h"
#include "collision/narrowphase/algorithm/epa/result/EPAResultNoCollide.h"
#include "collision/narrowphase/algorithm/continuous/result/ContinuousCollisionResult.h"

namespace urchin {

    class AlgorithmResultAllocator : public Singleton<AlgorithmResultAllocator> {
        public:
            friend class Singleton<AlgorithmResultAllocator>;

            SyncFixedSizePool<AlgorithmResult> *getAlgorithmResultPool() const;

            template<class T> std::unique_ptr<GJKResult<T>, AlgorithmResultDeleter> newGJKResultCollide(const Simplex<T>&);
            template<class T> std::unique_ptr<GJKResult<T>, AlgorithmResultDeleter> newGJKResultInvalid();
            template<class T> std::unique_ptr<GJKResult<T>, AlgorithmResultDeleter> newGJKResultNoCollide(T, const Simplex<T>&);

            template<class T> std::unique_ptr<EPAResult<T>, AlgorithmResultDeleter> newEPAResultCollide(const Point3<T>&, const Point3<T>&, const Vector3<T>&, T);
            template<class T> std::unique_ptr<EPAResult<T>, AlgorithmResultDeleter> newEPAResultInvalid();
            template<class T> std::unique_ptr<EPAResult<T>, AlgorithmResultDeleter> newEPAResultNoCollide();

            template<class T> std::unique_ptr<ContinuousCollisionResult<T>, AlgorithmResultDeleter> newContinuousCollisionResult(AbstractWorkBody*, const Vector3<T>&, const Point3<T>&, T);

        private:
            AlgorithmResultAllocator();
            ~AlgorithmResultAllocator() override;

            SyncFixedSizePool<AlgorithmResult> *algorithmResultPool;
    };

}

#endif
