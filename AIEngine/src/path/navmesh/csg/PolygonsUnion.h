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
			std::vector<CSGPolygon> unionPolygons(const std::vector<CSGPolygon> &) const;

		private:
			std::vector<CSGPolygon> unionTwoPolygons(const CSGPolygon &, const CSGPolygon &) const;
			unsigned int findStartPoint(const CSGPolygon &, const CSGPolygon &, const CSGPolygon *&) const;
			unsigned int findLowestPointIndex(const CSGPolygon &) const;
			CSGIntersection findFirstIntersectionOnEdge(const LineSegment2D<float> &, const CSGPolygon *) const;
			bool pointInsidePolygon(const CSGPolygon *, const Point2<float> &) const;
	};

}

#endif
