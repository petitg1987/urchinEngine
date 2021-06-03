#ifndef URCHINENGINE_ALGORITHMRESULTDELETER_H
#define URCHINENGINE_ALGORITHMRESULTDELETER_H

#include <utils/pool/FixedSizePool.h>
#include <collision/narrowphase/algorithm/util/AlgorithmResult.h>

namespace urchin {

    class AlgorithmResultDeleter {
        public:
            void operator()(AlgorithmResult*);
    };

}

#endif
