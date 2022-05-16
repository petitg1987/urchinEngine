#pragma once

#include <UrchinCommon.h>

#include <utils/pool/SyncFixedSizePool.h>
#include <body/model/AbstractBody.h>
#include <collision/narrowphase/algorithm/util/AlgorithmResult.h>
#include <collision/narrowphase/algorithm/util/AlgorithmResultDeleter.h>
#include <collision/narrowphase/algorithm/epa/result/EPAResultCollide.h>
#include <collision/narrowphase/algorithm/epa/result/EPAResultInvalid.h>
#include <collision/narrowphase/algorithm/epa/result/EPAResultNoCollide.h>
#include <collision/narrowphase/algorithm/continuous/ContinuousCollisionResult.h>

namespace urchin {

    class AlgorithmResultAllocator : public ThreadSafeSingleton<AlgorithmResultAllocator> {
        public:
            friend class ThreadSafeSingleton<AlgorithmResultAllocator>;

            SyncFixedSizePool<AlgorithmResult>& getAlgorithmResultPool() const;

            template<class T> std::unique_ptr<EPAResult<T>, AlgorithmResultDeleter> newEPAResultCollide(const Point3<T>&, const Point3<T>&, const Vector3<T>&, T);
            template<class T> std::unique_ptr<EPAResult<T>, AlgorithmResultDeleter> newEPAResultInvalid();
            template<class T> std::unique_ptr<EPAResult<T>, AlgorithmResultDeleter> newEPAResultNoCollide();

        private:
            AlgorithmResultAllocator();

            std::unique_ptr<SyncFixedSizePool<AlgorithmResult>> algorithmResultPool;
    };

}
