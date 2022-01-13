#pragma once

#include <limits>
#include <vector>

namespace urchin {

    class MathFunction {
        public:
            MathFunction() = delete;

            template<class T> static T clamp(T, T, T);
            template<class T> static T sign(T);
            template<class T> static T lerp(T, T, T);
            template<class T> static T max(const std::vector<T>&);

            static unsigned int powerOfTwo(unsigned int);
            static int pow(int, unsigned int);

            static bool isEqual(float, float, float tolerance = std::numeric_limits<float>::epsilon());
            static bool isEqual(double, double, double tolerance = std::numeric_limits<double>::epsilon());
            static bool isZero(float, float tolerance = std::numeric_limits<float>::epsilon());
            static bool isZero(double, double tolerance = std::numeric_limits<double>::epsilon());
            static bool isOne(float, float tolerance = std::numeric_limits<float>::epsilon());
            static bool isOne(double, double tolerance = std::numeric_limits<double>::epsilon());

            static int roundToInt(float);
            static unsigned int roundToUInt(float);
            static int roundToInt(double);
            static unsigned int roundToUInt(double);
            static int ceilToInt(float);
            static unsigned int ceilToUInt(float);
            static int ceilToInt(double);
            static unsigned int ceilToUInt(double);
            static int floorToInt(float);
            static unsigned int floorToUInt(float);
            static int floorToInt(double);
            static unsigned int floorToUInt(double);
            template<class T> static T roundDivision(T, T);
    };

}
