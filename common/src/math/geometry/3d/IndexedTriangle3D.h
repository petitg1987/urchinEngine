#ifndef URCHINENGINE_INDEXEDTRIANGLE3D_H
#define URCHINENGINE_INDEXEDTRIANGLE3D_H

#include <vector>
#include <map>

#include "math/algebra/point/Point3.h"
#include "math/algebra/vector/Vector3.h"

namespace urchin
{

	template<class T> class IndexedTriangle3D
	{
		public:
			explicit IndexedTriangle3D(const unsigned int *);
			explicit IndexedTriangle3D(unsigned int, unsigned int, unsigned int);

			const unsigned int *getIndices() const;
			unsigned int getIndex(unsigned int) const;
			Vector3<T> computeNormal(const std::vector<Point3<T>> &) const;
			Vector3<T> computeNormal(const Point3<T> &, const Point3<T> &, const Point3<T> &) const;

		private:
			unsigned int indices[3];
	};

}

#endif
