#ifndef URCHINENGINE_ANGLECONVERTER_H
#define URCHINENGINE_ANGLECONVERTER_H

#include "math/algebra/MathValue.h"
#include "pattern/singleton/Singleton.h"

namespace urchin {

    template<class T> class AngleConverter : public Singleton<AngleConverter<T>> {
        public:
            friend class Singleton<AngleConverter>;

            T toDegree(T);
            T toRadian(T);

        private:
            AngleConverter() = default;
            ~AngleConverter() override = default;

            static const T RADIAN_TO_DEGREE_FACTOR;
            static const T DEGREE_TO_RADIAN_FACTOR;
    };

}

#endif
