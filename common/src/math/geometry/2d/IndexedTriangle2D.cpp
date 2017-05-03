#include "IndexedTriangle2D.h"

namespace urchin
{

	template<class T> IndexedTriangle2D<T>::IndexedTriangle2D(const unsigned int *indexes)
	{
		this->indexes[0] = indexes[0];
		this->indexes[1] = indexes[1];
		this->indexes[2] = indexes[2];
	}

	template<class T> IndexedTriangle2D<T>::IndexedTriangle2D(unsigned int index1, unsigned int index2, unsigned int index3)
	{
		this->indexes[0] = index1;
		this->indexes[1] = index2;
		this->indexes[2] = index3;
	}

	template<class T> const unsigned int *IndexedTriangle2D<T>::getIndexes() const
	{
		return indexes;
	}

	template<class T> unsigned int IndexedTriangle2D<T>::getIndex(unsigned int index) const
	{
		return indexes[index];
	}

	//explicit template
	template class IndexedTriangle2D<float>;
	template class IndexedTriangle2D<double>;

}
