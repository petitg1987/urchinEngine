#ifndef URCHINENGINE_POLYGONSINTERSECTION_H
#define URCHINENGINE_POLYGONSINTERSECTION_H

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
			~PolygonsIntersection() override = default;

			bool isPointInside(const Line2D<T> &, const Point2<T> &) const;
			void addIntersection(const Line2D<T> &, const Line2D<T> &, std::vector<Point2<T>> &) const;

			void logInputData(const CSGPolygon<T> &, const CSGPolygon<T> &, const std::string &, Logger::CriticalityLevel) const;
	};

}

#endif
