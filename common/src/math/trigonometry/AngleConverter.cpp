#include "AngleConverter.h"

namespace urchin {

    //static
    template<class T> T const AngleConverter<T>::RADIAN_TO_DEGREE_FACTOR = (T)180.0 / (T)MathValue::PI;
    template<class T> T const AngleConverter<T>::DEGREE_TO_RADIAN_FACTOR = (T)MathValue::PI / (T)180.0;

    template<class T> T AngleConverter<T>::toDegree(T radian) {
        return radian * RADIAN_TO_DEGREE_FACTOR;
    }

    template<class T> T AngleConverter<T>::toRadian(T degree) {
        return degree * DEGREE_TO_RADIAN_FACTOR;
    }

    //explicit template
    template class AngleConverter<float>;
    template class AngleConverter<double>;

}
