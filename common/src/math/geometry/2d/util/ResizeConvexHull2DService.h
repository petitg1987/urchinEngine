#ifndef ENGINE_RESIZECONVEXHULL2DSERVICE_H
#define ENGINE_RESIZECONVEXHULL2DSERVICE_H

#include <memory>
#include <vector>

#include "pattern/singleton/Singleton.h"
#include "math/geometry/2d/shape/ConvexHullShape2D.h"
#include "math/geometry/2d/object/ConvexHull2D.h"

namespace urchin
{

	template<class T> class ResizeConvexHull2DService : public Singleton<ResizeConvexHull2DService<T>>
	{
		public:
			friend class Singleton<ResizeConvexHull2DService<T>>;

			std::unique_ptr<ConvexHullShape2D<T>> resizeConvexHullShape(const ConvexHullShape2D<T> &, T) const;
			std::unique_ptr<ConvexHull2D<T>> resizeConvexHull(const ConvexHull2D<T> &, T) const;

		private:
			ResizeConvexHull2DService();
			virtual ~ResizeConvexHull2DService() = default;
	};

}

#endif
