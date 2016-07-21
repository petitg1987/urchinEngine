#include "collision/narrowphase/algorithm/raycast/RayCastResult.h"

namespace urchin
{

	template<class T> RayCastResult<T>::RayCastResult()
	{

	}

	template<class T> RayCastResult<T>::~RayCastResult()
	{

	}

	//explicit template
	template class RayCastResult<float>;
	template class RayCastResult<double>;

}
