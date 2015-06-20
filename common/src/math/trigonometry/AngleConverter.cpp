#include "math/trigonometry/AngleConverter.h"

namespace urchin
{

	template<class T> AngleConverter<T>::AngleConverter()
	{

	}

	template<class T> AngleConverter<T>::~AngleConverter()
	{

	}

	template<class T> T AngleConverter<T>::toDegree(T radian)
	{
		return radian * RADIAN_TO_DEGREE_FACTOR;
	}

	template<class T> T AngleConverter<T>::toRadian(T degree)
	{
		return degree * DEGREE_TO_RADIAN_FACTOR;
	}

	//explicit template
	template class AngleConverter<float>;
	template class AngleConverter<double>;

}
