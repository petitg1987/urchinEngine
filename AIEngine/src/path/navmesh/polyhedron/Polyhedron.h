#ifndef ENGINE_POLYHEDRON_H
#define ENGINE_POLYHEDRON_H

#include <vector>
#include <memory>
#include "UrchinCommon.h"

#include "path/navmesh/polyhedron/PolyhedronFace.h"
#include "path/navmesh/polyhedron/PolyhedronPoint.h"
#include "path/navmesh/csg/CSGConvexPolygon.h"

namespace urchin
{

	class Polyhedron
	{
		public:
			Polyhedron(const std::string &, std::vector<PolyhedronFace> &, const std::vector<PolyhedronPoint> &);

			const std::string getName() const;

			const std::vector<PolyhedronFace> &getFaces() const;
			const PolyhedronFace &getFace(unsigned int) const;

			std::shared_ptr<CSGConvexPolygon> getOrComputeCSGConvexPolygon() const;

			void setWalkableCandidate(bool);
			bool isWalkableCandidate() const;

			void expand(const BoxShape<float> &);

		private:
			std::vector<Plane<float>> buildPlanesFromFaces() const;
			void shiftPlanes(std::vector<Plane<float>> &, const BoxShape<float> &) const;
			float computeShiftDistance(const Vector3<float> &, const BoxShape<float> &) const;
			std::vector<Plane<float>> findThreeNonParallelPlanes(const std::vector<unsigned int> &, const std::vector<Plane<float>> &) const;

			std::vector<Point2<float>> flatPointsOnYAxis() const;

			std::string name;
			std::vector<PolyhedronFace> faces;
			std::vector<PolyhedronPoint> points;
			mutable std::shared_ptr<CSGConvexPolygon> csgConvexPolygon;

			bool walkableCandidate;
	};

}

#endif
