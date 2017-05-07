#ifndef ENGINE_POLYHEDRON_H
#define ENGINE_POLYHEDRON_H

#include <vector>
#include <memory>

#include "path/navmesh/polyhedron/Face.h"

namespace urchin
{

	struct PolyhedronPoint
	{
		Point3<float> point;
		std::vector<unsigned int> faceIndices;
	};

	class Polyhedron
	{
		public:
			Polyhedron(const std::string &, const std::vector<Face> &, const std::vector<PolyhedronPoint> &);

			const std::string getName() const;

			const std::vector<Face> &getFaces() const;
			const Face &getFace(unsigned int) const;

			void setWalkableCandidate(bool);
			bool isWalkableCandidate() const;

			std::vector<Point2<float>> computeCwFootprintPoints(const BoxShape<float> &) const;

		private:
			std::string name;
			std::vector<Face> faces;
			std::vector<PolyhedronPoint> points;

			bool walkableCandidate;

			std::vector<Point2<float>> flatPointsOnYAxis(const std::vector<PolyhedronPoint> &) const;
	};

}

#endif
