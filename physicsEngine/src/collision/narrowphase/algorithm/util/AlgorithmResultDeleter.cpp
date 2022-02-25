#include <collision/narrowphase/algorithm/util/AlgorithmResultDeleter.h>
#include <collision/narrowphase/algorithm/util/AlgorithmResultAllocator.h>

namespace urchin {

    void AlgorithmResultDeleter::operator()(AlgorithmResult *const collisionResultAlgorithm) const {
        AlgorithmResultAllocator::instance().getAlgorithmResultPool().deallocate(collisionResultAlgorithm);
    }

}
