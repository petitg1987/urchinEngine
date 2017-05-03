#ifndef ENGINE_RESIZECONVEXHULL3DSERVICE_H
#define ENGINE_RESIZECONVEXHULL3DSERVICE_H

#include <memory>
#include <map>
#include <vector>

#include "pattern/singleton/Singleton.h"
#include "math/geometry/3d/shape/ConvexHullShape3D.h"
#include "math/geometry/3d/object/ConvexHull3D.h"
#include "math/geometry/3d/Plane.h"
#include "math/algebra/point/Point3.h"

namespace urchin
{

	template<class T> class ResizeConvexHull3DService : public Singleton<ResizeConvexHull3DService<T>>
	{
		public:
			friend class Singleton<ResizeConvexHull3DService<T>>;

			std::unique_ptr<ConvexHullShape3D<T>> resizeConvexHullShape(const ConvexHullShape3D<T> &, T) const;
			std::unique_ptr<ConvexHull3D<T>> resizeConvexHull(const ConvexHull3D<T> &, T) const;

		private:
			ResizeConvexHull3DService();
			virtual ~ResizeConvexHull3DService();

			std::map<unsigned int, Plane<T>> buildPlanesFromConvexHullShape(const ConvexHullShape3D<T> &) const;
			void shiftPlanes(std::map<unsigned int, Plane<T>> &, T) const;
			std::vector<Plane<T>> findThreeNonParallelPlanes(const std::vector<unsigned int> &, const std::map<unsigned int, Plane<T>> &) const;
			bool isPointInsidePlanes(const std::map<unsigned int, Plane<T>> &, const Point3<T> &) const;
	};

}

#endif
