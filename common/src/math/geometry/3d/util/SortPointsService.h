#ifndef ENGINE_SORTPOINTSSERVICE_H
#define ENGINE_SORTPOINTSSERVICE_H

#include <vector>

#include "pattern/singleton/Singleton.h"
#include "math/algebra/point/Point3.h"
#include "math/algebra/vector/Vector3.h"

namespace urchin
{

	template<class T> class SortPointsService : public Singleton<SortPointsService<T>>
	{
		public:
			friend class Singleton<SortPointsService<T>>;

			std::vector<Point3<T>> sortPointsClockwise(const std::vector<Point3<T>> &, const Vector3<T> &);
			std::vector<Point3<T>> sortPointsCounterClockwise(const std::vector<Point3<T>> &, const Vector3<T> &);

		private:
			SortPointsService();
			virtual ~SortPointsService();

			bool isNewPointClockwiseSorted(const std::vector<Point3<T>> &, const Vector3<T> &, unsigned int);
	};

}

#endif
