#include <cassert>
#include <algorithm>
#include <cmath>

#include "math/algorithm/MathFunction.h"

namespace urchin {

    template<class T> T MathFunction::clamp(T value, T minValue, T maxValue) {
        assert((maxValue + std::numeric_limits<T>::epsilon()) > minValue);

        return std::max(minValue, std::min(maxValue, value));
    }

    /**
     * @return -1 if value is negative, 0 if value is 0, otherwise 1
     */
    template<class T> T MathFunction::sign(T value) {
        return (T)((T)0.0 < value) - (value < (T)0.0);
    }

    template<class T> T MathFunction::lerp(T v0, T v1, T t) { //see https://en.wikipedia.org/wiki/Linear_interpolation
        return ((T)1.0 - t) * v0 + t * v1;
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

    bool MathFunction::isZero(float value, float tolerance) {
        return value > 0.0f - tolerance && value < 0.0f + tolerance;
    }

    bool MathFunction::isOne(float value, float tolerance) {
        return value > 1.0f - tolerance && value < 1.0f + tolerance;
    }

    int MathFunction::roundToInt(float value) {
        return (int)std::lround(value);
    }

    unsigned int MathFunction::roundToUInt(float value) {
        return (unsigned int)std::lround(value);
    }

    int MathFunction::ceilToInt(float value) {
        return (int)std::ceil(value);
    }

    unsigned int MathFunction::ceilToUInt(float value) {
        return (unsigned int)std::ceil(value);
    }

    int MathFunction::floorToInt(float value) {
        return (int)std::floor(value);
    }

    unsigned int MathFunction::floorToUInt(float value) {
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
    template float MathFunction::clamp<float>(float, float, float);
    template float MathFunction::sign<float>(float);
    template float MathFunction::lerp<float>(float, float, float);
    template float MathFunction::roundDivision<float>(float, float);

    template double MathFunction::clamp<double>(double, double, double);
    template double MathFunction::sign<double>(double);
    template double MathFunction::lerp<double>(double, double, double);
    template double MathFunction::roundDivision<double>(double, double);

    template int MathFunction::clamp<int>(int, int, int);
    template int MathFunction::sign<int>(int);
    template int MathFunction::lerp<int>(int, int, int);
    template int MathFunction::roundDivision<int>(int, int);

    template unsigned int MathFunction::clamp<unsigned int>(unsigned int, unsigned int, unsigned int);
    template unsigned int MathFunction::lerp<unsigned int>(unsigned int, unsigned int, unsigned int);
    template unsigned int MathFunction::roundDivision<unsigned int>(unsigned int, unsigned int);

    template long long MathFunction::clamp<long long>(long long, long long, long long);
    template long long MathFunction::sign<long long>(long long);
    template long long MathFunction::roundDivision<long long>(long long, long long);
}
