#ifndef ENGINE_POLYGONSINTERSECTION_H
#define ENGINE_POLYGONSINTERSECTION_H

#include "CSGConvexPolygon.h"

namespace urchin
{

	class PolygonsIntersection
	{
		public:
			PolygonsIntersection();

			CSGPolygon intersectionPolygons(const CSGConvexPolygon &, const CSGConvexPolygon &) const;
	};

}

#endif
