#ifndef URCHINENGINE_POLYHEDRON_H
#define URCHINENGINE_POLYHEDRON_H

#include <vector>
#include <sstream>
#include <memory>
#include "UrchinCommon.h"

#include "path/navmesh/polyhedron/PolyhedronFace.h"
#include "path/navmesh/polyhedron/PolyhedronPoint.h"
#include "path/navmesh/model/NavMeshAgent.h"

namespace urchin
{

	class Polyhedron
	{
		public:
			Polyhedron();
			Polyhedron(const std::string &, std::vector<PolyhedronFace> &, const std::vector<PolyhedronPoint> &);

			const std::string getName() const;

			const std::vector<PolyhedronFace> &getFaces() const;
			const PolyhedronFace &getFace(unsigned int) const;
			const std::vector<PolyhedronPoint> &getPoints() const;

			void setWalkableCandidate(bool);
			bool isWalkableCandidate() const;

			void setObstacleCandidate(bool);
			bool isObstacleCandidate() const;

			void expand(const NavMeshAgent &);

		private:
			std::vector<Plane<float>> buildPlanesFromFaces() const;
			void shiftPlanes(std::vector<Plane<float>> &, const NavMeshAgent &) const;
			std::vector<Plane<float>> findThreeNonParallelPlanes(const std::vector<unsigned int> &, const std::vector<Plane<float>> &) const;

			std::string name;
			std::vector<PolyhedronFace> faces;
			std::vector<PolyhedronPoint> points;

			bool walkableCandidate;
			bool obstacleCandidate;
	};

	std::ostream& operator <<(std::ostream &, const Polyhedron &);

}

#endif
