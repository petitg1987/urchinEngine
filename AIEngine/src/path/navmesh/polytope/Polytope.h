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
			Polytope(const std::string &, std::vector<std::unique_ptr<PolytopeSurface>> &, const std::vector<PolytopePoint> &points = std::vector<PolytopePoint>());

			const std::string getName() const;

			const std::vector<std::unique_ptr<PolytopeSurface>> &getSurfaces() const;
			const std::unique_ptr<PolytopeSurface> &getSurface(unsigned int) const;

			const std::unique_ptr<Rectangle<float>> &getXZRectangle() const;

			void setWalkableCandidate(bool);
			bool isWalkableCandidate() const;

			void setObstacleCandidate(bool);
			bool isObstacleCandidate() const;

            std::unique_ptr<Polytope> expand(const NavMeshAgent &) const;

		private:
            std::unique_ptr<Polytope> expandPolyhedron(const NavMeshAgent &) const;
			std::vector<Plane<float>> buildPlanesFromPlaneSurfaces() const;
			void shiftPlanes(std::vector<Plane<float>> &, const NavMeshAgent &) const;
			std::vector<Plane<float>> findThreeNonParallelPlanes(const std::vector<unsigned int> &, const std::vector<Plane<float>> &) const;
			std::unique_ptr<Polytope> expandPolygon(const NavMeshAgent &) const;

			void buildXZRectangle();

			std::string name;
			std::vector<std::unique_ptr<PolytopeSurface>> surfaces;
			std::vector<PolytopePoint> points;

			std::unique_ptr<Rectangle<float>> xzRectangle;

			bool walkableCandidate;
			bool obstacleCandidate;
	};

	std::ostream& operator <<(std::ostream &, const Polytope &);

}

#endif
