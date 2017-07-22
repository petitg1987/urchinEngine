#ifndef ENGINE_POLYGONSINTERSECTION_H
#define ENGINE_POLYGONSINTERSECTION_H

#include "UrchinCommon.h"

#include "CSGPolygon.h"

namespace urchin
{

	template<class T> class PolygonsIntersection : public Singleton<PolygonsIntersection<T>>
	{
		public:
			friend class Singleton<PolygonsIntersection<T>>;

			CSGPolygon<T> intersectionPolygons(const CSGPolygon<T> &, const CSGPolygon<T> &) const;

		private:
			PolygonsIntersection() = default;
			virtual ~PolygonsIntersection() = default;

			bool isPointInside(const Line2D<T> &, const Point2<T> &) const;
			void addIntersection(const Line2D<T> &, const Line2D<T> &, std::vector<Point2<T>> &) const;
	};

}

#endif
