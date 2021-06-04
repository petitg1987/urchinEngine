#pragma once

#include <math/algebra/MathValue.h>

namespace urchin {

    template<class T> class AngleConverter {
        public:
            AngleConverter() = delete;

            static T toDegree(T);
            static T toRadian(T);

        private:
            static const T RADIAN_TO_DEGREE_FACTOR;
            static const T DEGREE_TO_RADIAN_FACTOR;
    };

}
