#ifndef ENGINE_INDEXEDTRIANGLE2D_H
#define ENGINE_INDEXEDTRIANGLE2D_H

#include <vector>
#include <map>

#include "math/algebra/point/Point2.h"
#include "math/algebra/vector/Vector2.h"

namespace urchin
{

	template<class T> class IndexedTriangle2D
	{
		public:
			IndexedTriangle2D(const unsigned int *);
			IndexedTriangle2D(unsigned int, unsigned int, unsigned int);

			const unsigned int *getIndexes() const;

		private:
			unsigned int indexes[3];
	};

}

#endif
