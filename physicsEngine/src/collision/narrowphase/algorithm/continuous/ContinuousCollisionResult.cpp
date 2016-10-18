#include "collision/narrowphase/algorithm/continuous/ContinuousCollisionResult.h"

namespace urchin
{

	template<class T> ContinuousCollisionResult<T>::ContinuousCollisionResult()
	{

	}

	template<class T> ContinuousCollisionResult<T>::~ContinuousCollisionResult()
	{

	}

	//explicit template
	template class ContinuousCollisionResult<float>;
	template class ContinuousCollisionResult<double>;

}
