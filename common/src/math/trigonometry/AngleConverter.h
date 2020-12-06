#ifndef URCHINENGINE_ANGLECONVERTER_H
#define URCHINENGINE_ANGLECONVERTER_H

#include "math/algebra/MathValue.h"

namespace urchin {

    template<class T> class AngleConverter {
        #define RADIAN_TO_DEGREE_FACTOR (180.0 / MathValue::PI)
        #define DEGREE_TO_RADIAN_FACTOR (MathValue::PI / 180.0)

        public:
            static T toDegree(T);
            static T toRadian(T);

        private:
            AngleConverter() = default;
            ~AngleConverter() = default;
    };

}

#endif
