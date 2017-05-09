#ifndef ENGINE_NAVMESHGENERATOR_H
#define ENGINE_NAVMESHGENERATOR_H

#include <memory>
#include <vector>

#include "input/AIWorld.h"
#include "path/navmesh/NavMeshConfig.h"
#include "path/navmesh/model/NavMesh.h"
#include "path/navmesh/model/NavPolygon.h"
#include "path/navmesh/polyhedron/Polyhedron.h"
#include "path/navmesh/polyhedron/PolyhedronFace.h"
#include "path/navmesh/polyhedron/PolyhedronPoint.h"
#include "path/navmesh/triangulation/Triangulation.h"

namespace urchin
{

	struct PolyhedronFaceIndex
	{
		unsigned int polyhedronIndex;
		unsigned int faceIndex;
	};

	class NavMeshGenerator
	{
		public:
			NavMeshGenerator(std::shared_ptr<AIWorld>, NavMeshConfig);

			std::shared_ptr<NavMesh> generate() const;

		private:
			std::vector<Polyhedron> createExpandedPolyhedrons() const;
			std::vector<PolyhedronPoint> createPolyhedronPoints(OBBox<float> *) const;
			std::vector<PolyhedronFace> createPolyhedronFaces() const;
			Polyhedron createPolyhedronFor(const std::string &, OBBox<float> *) const;
			Polyhedron createPolyhedronFor(const std::string &, Capsule<float> *) const;
			Polyhedron createPolyhedronFor(const std::string &, Cone<float> *) const;
			Polyhedron createPolyhedronFor(const std::string &, ConvexHull3D<float> *) const;
			Polyhedron createPolyhedronFor(const std::string &, Cylinder<float> *) const;
			Polyhedron createPolyhedronFor(const std::string &, Sphere<float> *) const;

			void expandPolyhedrons(std::vector<Polyhedron> &) const;

			std::vector<PolyhedronFaceIndex> findWalkableFaces(const std::vector<Polyhedron> &) const;

			NavPolygon createNavigationPolygonFor(const PolyhedronFaceIndex &, const std::vector<Polyhedron> &) const;
			std::vector<Point2<float>> flatPointsOnYAxis(const std::vector<Point3<float>> &) const;
			void addObstacles(const std::vector<Polyhedron> &, unsigned int, const PolyhedronFace &, Triangulation &) const;
			std::vector<Point3<float>> elevateTriangulatedPoints(const Triangulation &, const PolyhedronFace &) const;
			std::vector<IndexedTriangle3D<float>> toIndexedTriangle3D(const std::vector<IndexedTriangle2D<float>> &) const;

			std::shared_ptr<AIWorld> aiWorld;
			NavMeshConfig navMeshConfig;
	};

}

#endif
