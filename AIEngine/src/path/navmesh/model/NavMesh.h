#ifndef ENGINE_NAVMESH_H
#define ENGINE_NAVMESH_H

#include <vector>
#include <memory>

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

			unsigned int addPolygon(std::shared_ptr<NavPolygon>);
			const std::vector<std::shared_ptr<NavPolygon>> &getPolygons() const;

		private:
			static unsigned int nextId;
			unsigned int id;

			std::vector<std::shared_ptr<NavPolygon>> polygons;
	};

}

#endif
