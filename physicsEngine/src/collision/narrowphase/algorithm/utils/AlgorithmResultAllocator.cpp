#include <algorithm>

#include "AlgorithmResultAllocator.h"

namespace urchin {

    AlgorithmResultAllocator::AlgorithmResultAllocator() {
        std::vector<unsigned int> resultSizes = {
                sizeof(ContinuousCollisionResult<double>),
                sizeof(EPAResultCollide<double>),
                sizeof(EPAResultInvalid<double>),
                sizeof(EPAResultNoCollide<double>),
                sizeof(GJKResultCollide<double>),
                sizeof(GJKResultInvalid<double>),
                sizeof(GJKResultNoCollide<double>)};
        unsigned int maxElementSize = *std::max_element(resultSizes.begin(), resultSizes.end());
        unsigned int algorithmPoolSize = ConfigService::instance()->getUnsignedIntValue("narrowPhase.algorithmPoolSize");

        //pool is synchronized because elements could be created/deleted by different threads as narrow phase can be called by different threads
        algorithmResultPool = new SyncFixedSizePool<AlgorithmResult>("algorithmResultPool", maxElementSize, algorithmPoolSize);
    }

    AlgorithmResultAllocator::~AlgorithmResultAllocator() {
        delete algorithmResultPool;
    }

    SyncFixedSizePool<AlgorithmResult> *AlgorithmResultAllocator::getAlgorithmResultPool() const {
        return algorithmResultPool;
    }

    template<class T> std::unique_ptr<GJKResult<T>, AlgorithmResultDeleter> AlgorithmResultAllocator::newGJKResultCollide(const Simplex<T> &simplex) {
        void *memPtr = AlgorithmResultAllocator::instance()->getAlgorithmResultPool()->allocate(sizeof(GJKResultCollide<T>));
        return std::unique_ptr<GJKResultCollide<T>, AlgorithmResultDeleter>(new(memPtr) GJKResultCollide<T>(simplex), AlgorithmResultDeleter());
    }

    template<class T> std::unique_ptr<GJKResult<T>, AlgorithmResultDeleter> AlgorithmResultAllocator::newGJKResultInvalid() {
        void *memPtr = AlgorithmResultAllocator::instance()->getAlgorithmResultPool()->allocate(sizeof(GJKResultInvalid<T>));
        return std::unique_ptr<GJKResultInvalid<T>, AlgorithmResultDeleter>(new(memPtr) GJKResultInvalid<T>(), AlgorithmResultDeleter());
    }

    template<class T> std::unique_ptr<GJKResult<T>, AlgorithmResultDeleter> AlgorithmResultAllocator::newGJKResultNoCollide(T separatingDistance, const Simplex<T> &simplex) {
        void *memPtr = AlgorithmResultAllocator::instance()->getAlgorithmResultPool()->allocate(sizeof(GJKResultNoCollide<T>));
        return std::unique_ptr<GJKResultNoCollide<T>, AlgorithmResultDeleter>(new(memPtr) GJKResultNoCollide<T>(separatingDistance, simplex), AlgorithmResultDeleter());
    }

    template<class T> std::unique_ptr<EPAResult<T>, AlgorithmResultDeleter> AlgorithmResultAllocator::newEPAResultCollide(const Point3<T> &contactPointA, const Point3<T> &contactPointB, const Vector3<T> &normal, T depth) {
        void *memPtr = AlgorithmResultAllocator::instance()->getAlgorithmResultPool()->allocate(sizeof(EPAResultCollide<T>));
        return std::unique_ptr<EPAResultCollide<T>, AlgorithmResultDeleter>(new(memPtr) EPAResultCollide<T>(contactPointA, contactPointB, normal, depth), AlgorithmResultDeleter());
    }

    template<class T> std::unique_ptr<EPAResult<T>, AlgorithmResultDeleter> AlgorithmResultAllocator::newEPAResultInvalid() {
        void *memPtr = AlgorithmResultAllocator::instance()->getAlgorithmResultPool()->allocate(sizeof(EPAResultInvalid<T>));
        return std::unique_ptr<EPAResultInvalid<T>, AlgorithmResultDeleter>(new(memPtr) EPAResultInvalid<T>(), AlgorithmResultDeleter());
    }

    template<class T> std::unique_ptr<EPAResult<T>, AlgorithmResultDeleter> AlgorithmResultAllocator::newEPAResultNoCollide() {
        void *memPtr = AlgorithmResultAllocator::instance()->getAlgorithmResultPool()->allocate(sizeof(EPAResultNoCollide<T>));
        return std::unique_ptr<EPAResultNoCollide<T>, AlgorithmResultDeleter>(new(memPtr) EPAResultNoCollide<T>(), AlgorithmResultDeleter());
    }

    template<class T> std::unique_ptr<ContinuousCollisionResult<T>, AlgorithmResultDeleter> AlgorithmResultAllocator::newContinuousCollisionResult(AbstractWorkBody *body2, const Vector3<T> &normalFromObject2, const Point3<T> &hitPointOnObject2, T timeToHit) {
        void *memPtr = AlgorithmResultAllocator::instance()->getAlgorithmResultPool()->allocate(sizeof(ContinuousCollisionResult<T>));
        return std::unique_ptr<ContinuousCollisionResult<T>, AlgorithmResultDeleter>(new(memPtr) ContinuousCollisionResult<T>(body2, normalFromObject2, hitPointOnObject2, timeToHit), AlgorithmResultDeleter());
    }

    //explicit template
    template class std::unique_ptr<GJKResult<float>, AlgorithmResultDeleter> AlgorithmResultAllocator::newGJKResultCollide(const Simplex<float> &);
    template class std::unique_ptr<GJKResult<float>, AlgorithmResultDeleter> AlgorithmResultAllocator::newGJKResultInvalid();
    template class std::unique_ptr<GJKResult<float>, AlgorithmResultDeleter> AlgorithmResultAllocator::newGJKResultNoCollide(float, const Simplex<float> &);
    template class std::unique_ptr<EPAResult<float>, AlgorithmResultDeleter> AlgorithmResultAllocator::newEPAResultCollide(const Point3<float> &, const Point3<float> &, const Vector3<float> &, float);
    template class std::unique_ptr<EPAResult<float>, AlgorithmResultDeleter> AlgorithmResultAllocator::newEPAResultInvalid();
    template class std::unique_ptr<EPAResult<float>, AlgorithmResultDeleter> AlgorithmResultAllocator::newEPAResultNoCollide();
    template class std::unique_ptr<ContinuousCollisionResult<float>, AlgorithmResultDeleter> AlgorithmResultAllocator::newContinuousCollisionResult(AbstractWorkBody *body2, const Vector3<float> &normalFromObject2, const Point3<float> &hitPointOnObject2, float timeToHit);

    template class std::unique_ptr<GJKResult<double>, AlgorithmResultDeleter> AlgorithmResultAllocator::newGJKResultCollide(const Simplex<double> &);
    template class std::unique_ptr<GJKResult<double>, AlgorithmResultDeleter> AlgorithmResultAllocator::newGJKResultInvalid();
    template class std::unique_ptr<GJKResult<double>, AlgorithmResultDeleter> AlgorithmResultAllocator::newGJKResultNoCollide(double, const Simplex<double> &);
    template class std::unique_ptr<EPAResult<double>, AlgorithmResultDeleter> AlgorithmResultAllocator::newEPAResultCollide(const Point3<double> &, const Point3<double> &, const Vector3<double> &, double);
    template class std::unique_ptr<EPAResult<double>, AlgorithmResultDeleter> AlgorithmResultAllocator::newEPAResultInvalid();
    template class std::unique_ptr<EPAResult<double>, AlgorithmResultDeleter> AlgorithmResultAllocator::newEPAResultNoCollide();
    template class std::unique_ptr<ContinuousCollisionResult<double>, AlgorithmResultDeleter> AlgorithmResultAllocator::newContinuousCollisionResult(AbstractWorkBody *body2, const Vector3<double> &normalFromObject2, const Point3<double> &hitPointOnObject2, double timeToHit);
}
