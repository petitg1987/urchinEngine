#pragma once

#include <math/algebra/MathValue.h>

namespace urchin {

    template<class T> class AngleConverter {
        public:
            AngleConverter() = delete;

            static T toDegree(T);
            static T toRadian(T);

        private:
            static constexpr T RADIAN_TO_DEGREE_FACTOR = (T)180.0 / (T)MathValue::PI;
            static constexpr T DEGREE_TO_RADIAN_FACTOR = (T)MathValue::PI / (T)180.0;
    };

}
