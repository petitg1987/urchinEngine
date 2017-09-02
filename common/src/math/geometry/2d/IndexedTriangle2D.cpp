#include <cassert>

#include "IndexedTriangle2D.h"

namespace urchin
{

	template<class T> IndexedTriangle2D<T>::IndexedTriangle2D(const unsigned int *indices)
	{
		#ifdef _DEBUG
			assert(indices[0]!=indices[1] && indices[0]!=indices[2] && indices[1]!=indices[2]);
		#endif

		this->indices[0] = indices[0];
		this->indices[1] = indices[1];
		this->indices[2] = indices[2];
	}

	template<class T> IndexedTriangle2D<T>::IndexedTriangle2D(unsigned int index1, unsigned int index2, unsigned int index3)
	{
		#ifdef _DEBUG
			assert(index1!=index2 && index1!=index3 && index2!=index3);
		#endif

		this->indices[0] = index1;
		this->indices[1] = index2;
		this->indices[2] = index3;
	}

	template<class T> const unsigned int *IndexedTriangle2D<T>::getIndices() const
	{
		return indices;
	}

	template<class T> unsigned int IndexedTriangle2D<T>::getIndex(unsigned int index) const
	{
		#ifdef _DEBUG
			assert(index <= 2);
		#endif

		return indices[index];
	}

	//explicit template
	template class IndexedTriangle2D<float>;

	template class IndexedTriangle2D<double>;

}
