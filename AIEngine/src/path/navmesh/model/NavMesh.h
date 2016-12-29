#ifndef ENGINE_NAVMESH_H
#define ENGINE_NAVMESH_H

#include <vector>

#include "path/navmesh/model/NavPolygon.h"

namespace urchin
{

	/**
	 * Navigation mesh of world which can be used to do path finding, etc.
	 */
	class NavMesh
	{
		public:
			NavMesh();

			unsigned int getId() const;

			unsigned int addPolygon(const NavPolygon &);
			const std::vector<NavPolygon> &getPolygons() const;
			const NavPolygon &getPolygon(unsigned int) const;

		private:
			static unsigned int nextId;
			unsigned int id;

			std::vector<NavPolygon> polygons;
	};

}

#endif
