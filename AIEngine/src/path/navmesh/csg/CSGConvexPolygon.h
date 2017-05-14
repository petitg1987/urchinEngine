#ifndef ENGINE_CSGCONVEXPOLYGON_H
#define ENGINE_CSGCONVEXPOLYGON_H

#include "path/navmesh/csg/CSGPolygon.h"

namespace urchin
{

	class CSGConvexPolygon : public CSGPolygon
	{
		public:
			CSGConvexPolygon(std::string name, const std::vector<Point2<float>> &);
			CSGConvexPolygon(const CSGConvexPolygon &);
			CSGConvexPolygon(CSGConvexPolygon &&);
	};

}

#endif
