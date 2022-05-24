#include <cassert>
#include <algorithm>
#include <cmath>

#include <math/algorithm/MathFunction.h>

namespace urchin {

    /**
     * @return -1 if value is negative, 0 if value is 0, otherwise 1
     */
    template<class T> T MathFunction::sign(T value) {
        if (value < 0) {
            return (T)-1;
        } else if (value > 0) {
            return 1;
        }
        return 0;
    }

    template<class T> T MathFunction::max(const std::vector<T>& values) {
        T maxValue = -std::numeric_limits<T>::max();
        for (T value : values) {
            if (value > maxValue) {
                maxValue = value;
            }
        }
        return maxValue;
    }

    unsigned int MathFunction::powerOfTwo(unsigned int exponent) {
        return 1u << exponent;
    }

    int MathFunction::pow(int base, unsigned int exp) {
        int result = 1;
        while (exp) {
            if (exp & 1u) {
                result *= base;
            }
            exp >>= 1u;
            base *= base;
        }

        return result;
    }

    /**
     * Return the approximate value of arc sinus. In worst case, the maximum error is < 0.22 radian (12 degrees).
     * When the value is a little bit above 1.0 or below -1.0 due to float imprecision: the value returned is correct (for std::asin, the value returned is nan).
     */
    float MathFunction::approximateAsin(float x) {
        //Use maclaurin series (formula: https://www.wolframalpha.com/input?i2d=true&i=maclaurin+series+asin%5C%2840%29x%5C%2841%29)
        float xPow2 = x * x;
        float xPow4 = xPow2 * xPow2;
        float xPow7 = xPow4 * xPow2 * x;

        return x
               + (1.0f / 6.0f) * xPow2 * x
               + (3.0f / 40.0f) * xPow4 * x
               + (5.0f / 112.0f) * xPow7
               + (35.0f / 1152.0f) * xPow7 * xPow2
               + (63.0f / 2816.0f) * xPow7 * xPow4
               + (231.0f / 13312.0f) * xPow7 * xPow4 * xPow2;
    }

    template<class T> bool MathFunction::isEqual(T value1, T value2, T tolerance) {
        return value1 >= value2 - tolerance && value1 <= value2 + tolerance;
    }

    template<class T> bool MathFunction::isZero(T value, T tolerance) {
        return isEqual(value, (T)0.0, tolerance);
    }

    template<class T> bool MathFunction::isOne(T value, T tolerance) {
        return isEqual(value, (T)1.0, tolerance);
    }

    int MathFunction::roundToInt(float value) {
        return (int)std::lround(value);
    }

    unsigned int MathFunction::roundToUInt(float value) {
        return (unsigned int)std::lround(value);
    }

    int MathFunction::roundToInt(double value) {
        return (int)std::lround(value);
    }

    unsigned int MathFunction::roundToUInt(double value) {
        return (unsigned int)std::lround(value);
    }

    int MathFunction::ceilToInt(float value) {
        return (int)std::ceil(value);
    }

    unsigned int MathFunction::ceilToUInt(float value) {
        return (unsigned int)std::ceil(value);
    }

    int MathFunction::ceilToInt(double value) {
        return (int)std::ceil(value);
    }

    unsigned int MathFunction::ceilToUInt(double value) {
        return (unsigned int)std::ceil(value);
    }

    int MathFunction::floorToInt(float value) {
        return (int)std::floor(value);
    }

    unsigned int MathFunction::floorToUInt(float value) {
        return (unsigned int)std::floor(value);
    }

    int MathFunction::floorToInt(double value) {
        return (int)std::floor(value);
    }

    unsigned int MathFunction::floorToUInt(double value) {
        return (unsigned int)std::floor(value);
    }

    /**
     * Perform a division of two integers with a classical rounding.
     */
    template<class T> T MathFunction::roundDivision(T dividend, T divisor) { //see https://stackoverflow.com/questions/17005364/dividing-two-integers-and-rounding-up-the-result-without-using-floating-point
        assert(typeid(int) == typeid(T) || typeid(long) == typeid(T) || typeid(long long) == typeid(T));

        return (dividend + (divisor / 2)) / divisor;
    }

    //explicit template
    template float MathFunction::sign<float>(float);
    template float MathFunction::max(const std::vector<float>&);
    template bool MathFunction::isEqual(float, float, float);
    template bool MathFunction::isZero(float, float);
    template bool MathFunction::isOne(float, float);
    template float MathFunction::roundDivision<float>(float, float);

    template double MathFunction::sign<double>(double);
    template bool MathFunction::isEqual(double, double, double);
    template bool MathFunction::isZero(double, double);
    template bool MathFunction::isOne(double, double);
    template double MathFunction::roundDivision<double>(double, double);

    template int MathFunction::sign<int>(int);
    template bool MathFunction::isEqual(int, int, int);
    template int MathFunction::roundDivision<int>(int, int);

    template unsigned int MathFunction::roundDivision<unsigned int>(unsigned int, unsigned int);

    template long MathFunction::sign<long>(long);
    template long MathFunction::roundDivision<long>(long, long);

    template unsigned long MathFunction::sign<unsigned long>(unsigned long);
    template unsigned long MathFunction::roundDivision<unsigned long>(unsigned long, unsigned long);

    template long long MathFunction::sign<long long>(long long);
    template bool MathFunction::isEqual(long long, long long, long long);
    template long long MathFunction::roundDivision<long long>(long long, long long);
}
