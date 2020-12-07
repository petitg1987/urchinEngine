#ifndef URCHINENGINE_ANGLECONVERTER_H
#define URCHINENGINE_ANGLECONVERTER_H

#include "math/algebra/MathValue.h"

namespace urchin {

    template<class T> class AngleConverter {
        static const T RADIAN_TO_DEGREE_FACTOR;
        static const T DEGREE_TO_RADIAN_FACTOR;

        public:
            static T toDegree(T);
            static T toRadian(T);

        private:
            AngleConverter() = default;
            ~AngleConverter() = default;
    };

}

#endif
