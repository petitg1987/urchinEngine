#include "BoxShape.h"

namespace urchin
{

	/**
	 * @param halfSizes Half sizes (width, height and depth) of box
	 */
	template<class T> BoxShape<T>::BoxShape(const Vector3<T> &halfSizes) :
			Shape3D(),
			halfSizes(halfSizes)
	{

	}

	template<class T> BoxShape<T>::~BoxShape()
	{

	}

	template<class T> const T BoxShape<T>::getHalfSize(unsigned int index) const
	{
		return halfSizes[index];
	}

	template<class T> const Vector3<T> &BoxShape<T>::getHalfSizes() const
	{
		return halfSizes;
	}

	//explicit template
	template class BoxShape<float>;
}
