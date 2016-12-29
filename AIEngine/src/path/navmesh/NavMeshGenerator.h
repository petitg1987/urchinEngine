#ifndef ENGINE_NAVMESHGENERATOR_H
#define ENGINE_NAVMESHGENERATOR_H

#include <memory>

#include "input/AIWorld.h"
#include "path/navmesh/NavMeshConfig.h"
#include "path/navmesh/model/NavMesh.h"
#include "path/navmesh/model/NavPolygon.h"

namespace urchin
{

	class NavMeshGenerator
	{
		public:
			NavMeshGenerator(std::shared_ptr<AIWorld>, NavMeshConfig);

			std::shared_ptr<NavMesh> generate();

		private:
			void createNavPolygons(std::shared_ptr<NavMesh>);
			void createNavPolygonFor(OBBox<float> *, std::shared_ptr<NavMesh>);

			std::vector<Point3<float>> sortPointsClockwise(const std::vector<Point3<float>> &, const Vector3<float> &);
			bool isNewPointClockwise(const std::vector<Point3<float>> &, const Vector3<float> &, unsigned int);

			std::shared_ptr<AIWorld> aiWorld;
			NavMeshConfig navMeshConfig;
	};

}

#endif
