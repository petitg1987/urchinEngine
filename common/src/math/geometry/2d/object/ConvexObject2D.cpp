#include "math/geometry/2d/object/ConvexObject2D.h"

namespace urchin
{

	template<class T> ConvexObject2D<T>::ConvexObject2D()
	{

	}

	template<class T> ConvexObject2D<T>::~ConvexObject2D()
	{

	}

	//explicit template
	template class ConvexObject2D<float>;

	template class ConvexObject2D<double>;

	template class ConvexObject2D<int>;

	template class ConvexObject2D<long long>;

}
