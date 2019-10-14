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
			explicit IndexedTriangle2D(const std::size_t *);
			IndexedTriangle2D(std::size_t, std::size_t, std::size_t);

			const std::size_t *getIndices() const;
            std::size_t getIndex(std::size_t) const;

		private:
            std::size_t indices[3];
	};

}

#endif
