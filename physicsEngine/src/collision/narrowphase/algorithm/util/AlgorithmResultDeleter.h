#pragma once

#include <utils/pool/FixedSizePool.h>
#include <collision/narrowphase/algorithm/util/AlgorithmResult.h>

namespace urchin {

    class AlgorithmResultDeleter {
        public:
            void operator()(AlgorithmResult*) const;
    };

}
