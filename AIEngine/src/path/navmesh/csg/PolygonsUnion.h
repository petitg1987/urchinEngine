#ifndef ENGINE_POLYGONSUNION_H
#define ENGINE_POLYGONSUNION_H

#include <vector>
#include "UrchinCommon.h"

#include "CSGPolygon.h"

namespace urchin
{

	struct CSGIntersection
	{
		bool hasIntersection;
		Point2<float> intersectionPoint;
		unsigned int edgeEndPointIndex;
	};

	class PolygonsUnion
	{
		public:
			PolygonsUnion();
			std::vector<CSGPolygon> unionPolygons(const std::vector<CSGPolygon> &) const;

		private:
			std::vector<CSGPolygon> unionTwoPolygons(const CSGPolygon &, const CSGPolygon &) const;

			unsigned int findStartPoint(const CSGPolygon &, const CSGPolygon &, const CSGPolygon *&) const;
			unsigned int findLowestPointIndex(const CSGPolygon &) const;
			CSGIntersection findFirstValidIntersectionOnEdge(const LineSegment2D<float> &, const CSGPolygon *) const;
			bool isExteriorAngleLess180(const Point2<float> &, const Point2<float> &, const Point2<float> &) const;

			bool areSamePoints(const CSGPolygon *, unsigned int, const CSGPolygon *, unsigned int) const;
			bool pointInsideOrOnPolygon(const CSGPolygon *, const Point2<float> &) const;

			void logMaximumIterationReach(const CSGPolygon &, const CSGPolygon &) const;

			const float epsilon;
	};

}

#endif
