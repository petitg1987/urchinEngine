#ifndef URCHINENGINE_MATHFUNCTION_H
#define URCHINENGINE_MATHFUNCTION_H

#include <limits>

#include "pattern/singleton/Singleton.h"

namespace urchin {

    class MathFunction : public Singleton<MathFunction> {
        public:
            friend class Singleton<MathFunction>;

            template<class T> T clamp(T, T, T);
            template<class T> T sign(T);
            template<class T> T lerp(T, T, T);

            unsigned int powerOfTwo(unsigned int);
            int pow(int, unsigned int);

            bool isZero(float, float tolerance = std::numeric_limits<float>::epsilon());
            bool isOne(float, float tolerance = std::numeric_limits<float>::epsilon());

            int roundToInt(float);
            unsigned int roundToUInt(float);
            int ceilToInt(float);
            unsigned int ceilToUInt(float);
            int floorToInt(float);
            unsigned int floorToUInt(float);
            template<class T> T roundDivision(T, T);

        private:
            MathFunction() = default;
            ~MathFunction() override = default;
    };

}

#endif
