#ifndef ENGINE_INDEXEDTRIANGLE3D_H
#define ENGINE_INDEXEDTRIANGLE3D_H

#include <vector>
#include <map>

#include "math/algebra/point/Point3.h"
#include "math/algebra/vector/Vector3.h"

namespace urchin
{

	template<class T> class IndexedTriangle3D
	{
		public:
			IndexedTriangle3D(const unsigned int *);
			IndexedTriangle3D(unsigned int, unsigned int, unsigned int);

			const unsigned int *getIndexes() const;
			Vector3<T> computeNormal(const std::vector<Point3<T>> &) const;
			Vector3<T> computeNormal(const Point3<T> &, const Point3<T> &, const Point3<T> &) const;

		private:
			unsigned int indexes[3];
	};

}

#endif
