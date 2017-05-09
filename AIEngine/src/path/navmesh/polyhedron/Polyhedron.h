#ifndef ENGINE_POLYHEDRON_H
#define ENGINE_POLYHEDRON_H

#include <vector>
#include <memory>

#include "path/navmesh/polyhedron/PolyhedronFace.h"
#include "path/navmesh/polyhedron/PolyhedronPoint.h"

namespace urchin
{

	class Polyhedron
	{
		public:
			Polyhedron(const std::string &, std::vector<PolyhedronFace> &, const std::vector<PolyhedronPoint> &);

			const std::string getName() const;

			const std::vector<PolyhedronFace> &getFaces() const;
			const PolyhedronFace &getFace(unsigned int) const;

			void setWalkableCandidate(bool);
			bool isWalkableCandidate() const;

			void expand(const BoxShape<float> &);
			std::vector<Point2<float>> computeCwFootprintPoints(const PolyhedronFace &) const;

		private:
			std::string name;
			std::vector<PolyhedronFace> faces;
			std::vector<PolyhedronPoint> points;

			bool walkableCandidate;

			std::vector<Point2<float>> flatPointsOnYAxis(const std::vector<PolyhedronPoint> &) const;
	};

}

#endif
