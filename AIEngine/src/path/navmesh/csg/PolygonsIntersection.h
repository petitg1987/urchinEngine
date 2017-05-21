#ifndef ENGINE_POLYGONSINTERSECTION_H
#define ENGINE_POLYGONSINTERSECTION_H

#include "UrchinCommon.h"

#include "CSGPolygon.h"

namespace urchin
{

	class PolygonsIntersection : public Singleton<PolygonsIntersection>
	{
		public:
			friend class Singleton<PolygonsIntersection>;

			CSGPolygon intersectionPolygons(const CSGPolygon &, const CSGPolygon &) const;

		private:
			PolygonsIntersection();
			virtual ~PolygonsIntersection();

			bool isPointInside(const Line2D<float> &, const Point2<float> &) const;
			void addIntersection(const Line2D<float> &, const Line2D<float> &, std::vector<Point2<float>> &) const;
	};

}

#endif
