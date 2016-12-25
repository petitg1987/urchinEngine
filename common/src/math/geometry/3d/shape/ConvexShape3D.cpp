#include "ConvexShape3D.h"

namespace urchin
{

	template<class T> ConvexShape3D<T>::ConvexShape3D()
	{

	}

	template<class T> ConvexShape3D<T>::~ConvexShape3D()
	{

	}

	//explicit template
	template class ConvexShape3D<float>;
	template class ConvexShape3D<double>;

}
