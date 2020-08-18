#ifndef URCHINENGINE_CUSTOMUNIFORM_H
#define URCHINENGINE_CUSTOMUNIFORM_H

#include "UrchinCommon.h"

namespace urchin
{

    /**
    * Allow to load custom uniform for model displayer
    */
    class CustomUniform
    {
        public:
            virtual ~CustomUniform() = default;

            virtual void loadCustomUniforms() = 0;

    };

}

#endif
