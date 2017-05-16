#ifndef ENGINE_POLYGONSINTERSECTION_H
#define ENGINE_POLYGONSINTERSECTION_H

#include "CSGPolygon.h"

namespace urchin
{

	class PolygonsIntersection
	{
		public:
			PolygonsIntersection();

			CSGPolygon intersectionPolygons(const CSGPolygon &, const CSGPolygon &) const;

		private:
			bool isPointInside(const Line2D<float> &, const Point2<float> &) const;
			void addIntersection(const Line2D<float> &, const Line2D<float> &, std::vector<Point2<float>> &) const;
	};

}

#endif
