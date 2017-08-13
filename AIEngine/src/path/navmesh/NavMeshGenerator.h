#ifndef ENGINE_NAVMESHGENERATOR_H
#define ENGINE_NAVMESHGENERATOR_H

#include <memory>
#include <vector>
#include "UrchinCommon.h"

#include "input/AIWorld.h"
#include "path/navmesh/model/NavMeshConfig.h"
#include "path/navmesh/model/NavMesh.h"
#include "path/navmesh/model/NavPolygon.h"
#include "path/navmesh/polyhedron/Polyhedron.h"
#include "path/navmesh/polyhedron/PolyhedronFace.h"
#include "path/navmesh/polyhedron/PolyhedronPoint.h"
#include "path/navmesh/csg/CSGPolygon.h"
#include "path/navmesh/triangulation/Triangulation.h"

namespace urchin
{

	struct PolyhedronFaceIndex
	{
		PolyhedronFaceIndex(unsigned int, unsigned int);

		unsigned int polyhedronIndex;
		unsigned int faceIndex;
	};

	struct WalkablePolygonData
	{
		WalkablePolygonData(const CSGPolygon<float> &, const std::vector<bool> &);

		CSGPolygon<float> walkablePolygon;
		std::vector<bool> isExternalPoints;
	};

	class NavMeshGenerator
	{
		public:
			void setNavMeshConfig(std::shared_ptr<NavMeshConfig>);

			std::shared_ptr<NavMesh> generate(std::shared_ptr<AIWorld>);
			std::shared_ptr<NavMesh> getNavMesh() const;

		private:
			std::vector<Polyhedron> createExpandedPolyhedrons(std::shared_ptr<AIWorld>) const;
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

			std::vector<std::shared_ptr<NavPolygon>> createNavigationPolygonFor(const PolyhedronFaceIndex &, const std::vector<Polyhedron> &) const;
			std::vector<Point2<float>> reversePoints(const std::vector<Point2<float>> &) const;
			std::vector<Point2<float>> reverseAndFlatPointsOnYAxis(const std::vector<Point3<float>> &) const;
			std::vector<CSGPolygon<float>> computeObstacles(const std::vector<Polyhedron> &, const PolyhedronFaceIndex &) const;
			CSGPolygon<float> computePolyhedronFootprint(const Polyhedron &, const PolyhedronFace &) const;
			std::vector<Point3<float>> elevateTriangulatedPoints(const Triangulation &, const PolyhedronFace &) const;
			Point3<float> elevatePoints(const Point2<float> &, float, const PolyhedronFace &) const;
			std::vector<IndexedTriangle3D<float>> toIndexedTriangle3D(const std::vector<IndexedTriangle2D<float>> &) const;

			std::shared_ptr<NavMeshConfig> navMeshConfig;
			std::shared_ptr<NavMesh> navMesh;
	};

}

#endif
