#ifndef ENGINE_NAVMESH_H
#define ENGINE_NAVMESH_H

#include <vector>

#include "path/navmesh/model/NavPolygon.h"

namespace urchin
{

	/**
	 * Navigation mesh of world which can be used to do path finding...
	 */
	class NavMesh
	{
		public:
			NavMesh();

			unsigned int addPolygon(const NavPolygon &);
			const std::vector<NavPolygon> &getPolygons() const;
			const NavPolygon &getPolygon(unsigned int) const;

		private:
			std::vector<NavPolygon> polygons;
	};

}

#endif
