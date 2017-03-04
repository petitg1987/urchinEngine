#ifndef ENGINE_NAVMESHGENERATOR_H
#define ENGINE_NAVMESHGENERATOR_H

#include <memory>
#include <vector>

#include "input/AIWorld.h"
#include "path/navmesh/NavMeshConfig.h"
#include "path/navmesh/model/NavMesh.h"
#include "path/navmesh/model/NavPolygon.h"
#include "path/navmesh/polyhedron/Polyhedron.h"
#include "path/navmesh/triangulation/Triangulation.h"

namespace urchin
{

	class NavMeshGenerator
	{
		public:
			NavMeshGenerator(std::shared_ptr<AIWorld>, NavMeshConfig);

			std::shared_ptr<NavMesh> generate();

		private:
			std::vector<Polyhedron> createPolyhedrons();
			Polyhedron createPolyhedronFor(const std::string &, OBBox<float> *);

			std::vector<Point2<float>> flatPointsOnYAxis(const std::vector<Point3<float>> &);
			void addObstacles(const std::vector<Polyhedron> &, unsigned int, Triangulation &);

			std::shared_ptr<AIWorld> aiWorld;
			NavMeshConfig navMeshConfig;
	};

}

#endif
