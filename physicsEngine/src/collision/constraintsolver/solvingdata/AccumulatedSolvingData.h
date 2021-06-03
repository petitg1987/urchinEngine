#ifndef URCHINENGINE_ACCUMULATEDSOLVINGDATA_H
#define URCHINENGINE_ACCUMULATEDSOLVINGDATA_H

#include <UrchinCommon.h>

namespace urchin {

    struct AccumulatedSolvingData {
        AccumulatedSolvingData();

        float accNormalImpulse;
        float accTangentImpulse;
    };

}

#endif
