#pragma once

#include <limits>

namespace urchin {

    class MathFunction {
        public:
            MathFunction() = delete;

            template<class T> static T clamp(T, T, T);
            template<class T> static T sign(T);
            template<class T> static T lerp(T, T, T);

            static unsigned int powerOfTwo(unsigned int);
            static int pow(int, unsigned int);

            static bool isZero(float, float tolerance = std::numeric_limits<float>::epsilon());
            static bool isOne(float, float tolerance = std::numeric_limits<float>::epsilon());

            static int roundToInt(float);
            static unsigned int roundToUInt(float);
            static int ceilToInt(float);
            static unsigned int ceilToUInt(float);
            static int floorToInt(float);
            static unsigned int floorToUInt(float);
            template<class T> static T roundDivision(T, T);
    };

}
