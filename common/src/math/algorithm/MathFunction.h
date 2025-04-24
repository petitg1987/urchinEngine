#pragma once

#include <limits>

namespace urchin {

    class MathFunction {
        public:
            MathFunction() = delete;

            template<class T> static T sign(T);

            static unsigned int nearestPowerOfTwo(unsigned int);
            static int pow(int, unsigned int);

            template<class T> static bool isEqual(T, T, T tolerance = std::numeric_limits<T>::epsilon());
            template<class T> static bool isZero(T, T tolerance = std::numeric_limits<T>::epsilon());
            template<class T> static bool isOne(T, T tolerance = std::numeric_limits<T>::epsilon());

            static int roundToInt(float);
            static unsigned int roundToUInt(float);
            static int roundToInt(double);
            static unsigned int roundToUInt(double);
            static int ceilToInt(float);
            static unsigned int ceilToUInt(float);
            static int ceilToInt(double);
            static unsigned int ceilToUInt(double);
            static int floorToInt(float);
            //info: do not create 'unsigned int floorToUInt(float/double)': too risky with -epsilon values
            static int floorToInt(double);
            template<class T> static T roundDivision(T, T);
    };

}
