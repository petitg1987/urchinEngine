#ifndef URCHINENGINE_INDEXEDTRIANGLE2D_H
#define URCHINENGINE_INDEXEDTRIANGLE2D_H

#include <vector>
#include <map>

#include "math/algebra/point/Point2.h"
#include "math/algebra/vector/Vector2.h"

namespace urchin
{

	template<class T> class IndexedTriangle2D
	{
		public:
			explicit IndexedTriangle2D(const unsigned int *);
			explicit IndexedTriangle2D(unsigned int, unsigned int, unsigned int);

			const unsigned int *getIndices() const;
			unsigned int getIndex(unsigned int) const;

		private:
			unsigned int indices[3];
	};

}

#endif
