#include <algorithm>

#include <collision/narrowphase/algorithm/util/AlgorithmResultAllocator.h>

namespace urchin {

    AlgorithmResultAllocator::AlgorithmResultAllocator() {
        std::vector<unsigned int> resultSizes = {
                sizeof(ContinuousCollisionResult<double>),
                sizeof(EPAResultCollide<double>),
                sizeof(EPAResultInvalid<double>),
                sizeof(EPAResultNoCollide<double>)};
        unsigned int maxElementSize = *std::ranges::max_element(resultSizes);
        unsigned int algorithmPoolSize = ConfigService::instance().getUnsignedIntValue("narrowPhase.algorithmPoolSize");

        //pool is synchronized because elements could be created/deleted by different threads as narrow phase can be called by different threads
        algorithmResultPool = std::make_unique<SyncFixedSizePool<AlgorithmResult>>("algorithmResultPool", maxElementSize, algorithmPoolSize);
    }

    SyncFixedSizePool<AlgorithmResult>& AlgorithmResultAllocator::getAlgorithmResultPool() const {
        return *algorithmResultPool;
    }

    template<class T> std::unique_ptr<EPAResult<T>, AlgorithmResultDeleter> AlgorithmResultAllocator::newEPAResultCollide(const Point3<T>& contactPointA, const Point3<T>& contactPointB, const Vector3<T>& normal, T depth) {
        void* memPtr = AlgorithmResultAllocator::instance().getAlgorithmResultPool().allocate(sizeof(EPAResultCollide<T>));
        return std::unique_ptr<EPAResultCollide<T>, AlgorithmResultDeleter>(new(memPtr) EPAResultCollide<T>(contactPointA, contactPointB, normal, depth), AlgorithmResultDeleter());
    }

    template<class T> std::unique_ptr<EPAResult<T>, AlgorithmResultDeleter> AlgorithmResultAllocator::newEPAResultInvalid() {
        void* memPtr = AlgorithmResultAllocator::instance().getAlgorithmResultPool().allocate(sizeof(EPAResultInvalid<T>));
        return std::unique_ptr<EPAResultInvalid<T>, AlgorithmResultDeleter>(new(memPtr) EPAResultInvalid<T>(), AlgorithmResultDeleter());
    }

    template<class T> std::unique_ptr<EPAResult<T>, AlgorithmResultDeleter> AlgorithmResultAllocator::newEPAResultNoCollide() {
        void* memPtr = AlgorithmResultAllocator::instance().getAlgorithmResultPool().allocate(sizeof(EPAResultNoCollide<T>));
        return std::unique_ptr<EPAResultNoCollide<T>, AlgorithmResultDeleter>(new(memPtr) EPAResultNoCollide<T>(), AlgorithmResultDeleter());
    }

    template<class T> std::unique_ptr<ContinuousCollisionResult<T>, AlgorithmResultDeleter> AlgorithmResultAllocator::newContinuousCollisionResult(std::shared_ptr<AbstractBody> body2, std::size_t shapeIndex, const Vector3<T>& normalFromObject2, const Point3<T>& hitPointOnObject2, T timeToHit) {
        void* memPtr = AlgorithmResultAllocator::instance().getAlgorithmResultPool().allocate(sizeof(ContinuousCollisionResult<T>));
        return std::unique_ptr<ContinuousCollisionResult<T>, AlgorithmResultDeleter>(new(memPtr) ContinuousCollisionResult<T>(std::move(body2), shapeIndex, normalFromObject2, hitPointOnObject2, timeToHit), AlgorithmResultDeleter());
    }

    //explicit template
    template class std::unique_ptr<EPAResult<float>, AlgorithmResultDeleter> AlgorithmResultAllocator::newEPAResultCollide(const Point3<float>&, const Point3<float>&, const Vector3<float>&, float);
    template class std::unique_ptr<EPAResult<float>, AlgorithmResultDeleter> AlgorithmResultAllocator::newEPAResultInvalid();
    template class std::unique_ptr<EPAResult<float>, AlgorithmResultDeleter> AlgorithmResultAllocator::newEPAResultNoCollide();
    template class std::unique_ptr<ContinuousCollisionResult<float>, AlgorithmResultDeleter> AlgorithmResultAllocator::newContinuousCollisionResult(std::shared_ptr<AbstractBody>, std::size_t, const Vector3<float>&, const Point3<float>&, float);

    template class std::unique_ptr<EPAResult<double>, AlgorithmResultDeleter> AlgorithmResultAllocator::newEPAResultCollide(const Point3<double>&, const Point3<double>&, const Vector3<double>&, double);
    template class std::unique_ptr<EPAResult<double>, AlgorithmResultDeleter> AlgorithmResultAllocator::newEPAResultInvalid();
    template class std::unique_ptr<EPAResult<double>, AlgorithmResultDeleter> AlgorithmResultAllocator::newEPAResultNoCollide();
    template class std::unique_ptr<ContinuousCollisionResult<double>, AlgorithmResultDeleter> AlgorithmResultAllocator::newContinuousCollisionResult(std::shared_ptr<AbstractBody>, std::size_t, const Vector3<double>&, const Point3<double>&, double);
}
