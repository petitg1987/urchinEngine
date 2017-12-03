#ifndef URCHINENGINE_POLYTOPE_H
#define URCHINENGINE_POLYTOPE_H

#include <vector>
#include <sstream>
#include <memory>
#include "UrchinCommon.h"

#include "path/navmesh/polytope/PolytopeSurface.h"
#include "path/navmesh/polytope/PolytopePoint.h"
#include "path/navmesh/model/NavMeshAgent.h"

namespace urchin
{

	class Polytope
	{
		public:
			Polytope();
			Polytope(const std::string &, std::vector<std::unique_ptr<PolytopeSurface>> &&, const std::vector<PolytopePoint> &);

			const std::string getName() const;

			const std::vector<std::unique_ptr<PolytopeSurface>> &getSurfaces() const;
			const std::unique_ptr<PolytopeSurface> &getSurface(unsigned int) const;
			const std::vector<PolytopePoint> &getPoints() const;

			const std::unique_ptr<AABBox<float>> &getAABBox() const;

			void setWalkableCandidate(bool);
			bool isWalkableCandidate() const;

			void setObstacleCandidate(bool);
			bool isObstacleCandidate() const;

			void expand(const NavMeshAgent &);

		private:
			void expandPolyhedron(const NavMeshAgent &);
			std::vector<Plane<float>> buildPlanesFromPlaneSurfaces() const;
			void shiftPlanes(std::vector<Plane<float>> &, const NavMeshAgent &) const;
			std::vector<Plane<float>> findThreeNonParallelPlanes(const std::vector<unsigned int> &, const std::vector<Plane<float>> &) const;

			std::string name;
			std::vector<std::unique_ptr<PolytopeSurface>> surfaces;
			std::vector<PolytopePoint> points;

			std::unique_ptr<AABBox<float>> aabbox;

			bool walkableCandidate;
			bool obstacleCandidate;
	};

	std::ostream& operator <<(std::ostream &, const Polytope &);

}

#endif
