#pragma once

#include <UrchinCommon.h>

namespace urchin {

    struct ImpulseSolvingData {
        ImpulseSolvingData();

        float friction; //friction factor: 0=no friction, 1=total friction

        float bias; //bias to avoid bodies penetrate and for restitution

        float normalImpulseDenominator;
        float tangentImpulseDenominator;
    };

}
