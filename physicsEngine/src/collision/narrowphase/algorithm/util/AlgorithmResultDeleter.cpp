#include "AlgorithmResultDeleter.h"
#include <collision/narrowphase/algorithm/util/AlgorithmResultAllocator.h>

namespace urchin {

    void AlgorithmResultDeleter::operator()(AlgorithmResult *const collisionResultAlgorithm) {
        AlgorithmResultAllocator::instance()->getAlgorithmResultPool()->free(collisionResultAlgorithm);
    }

}
