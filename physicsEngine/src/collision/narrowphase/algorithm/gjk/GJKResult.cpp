#include "collision/narrowphase/algorithm/gjk/GJKResult.h"

namespace urchin
{

	template<class T> GJKResult<T>::GJKResult()
	{

	}

	template<class T> GJKResult<T>::~GJKResult()
	{

	}

	//explicit template
	template class GJKResult<float>;
	template class GJKResult<double>;

}
