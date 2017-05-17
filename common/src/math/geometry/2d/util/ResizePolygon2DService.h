#ifndef ENGINE_RESIZEPOLYGON2DSERVICE_H
#define ENGINE_RESIZEPOLYGON2DSERVICE_H

#include <vector>

#include "pattern/singleton/Singleton.h"
#include "math/algebra/point/Point2.h"

namespace urchin
{

	template<class T> class ResizePolygon2DService : public Singleton<ResizePolygon2DService<T>>
	{
		public:
			friend class Singleton<ResizePolygon2DService<T>>;

			std::vector<Point2<T>> resizePolygon(const std::vector<Point2<T>> &, T) const;

		private:
			ResizePolygon2DService();
			virtual ~ResizePolygon2DService();
	};

}

#endif
