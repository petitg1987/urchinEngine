#ifndef ENGINE_POLYHEDRON_H
#define ENGINE_POLYHEDRON_H

#include <vector>
#include <memory>

#include "path/navmesh/polyhedron/Face.h"

namespace urchin
{

	class Polyhedron
	{
		public:
			Polyhedron(const std::string &, const std::vector<Face> &, const std::vector<Point3<float>> &);

			const std::string getName() const;

			const std::vector<Face> &getFaces() const;
			unsigned int getFaceSize() const;
			const Face &getFace(unsigned int) const;

			std::vector<Point2<float>> computeCwFootprintPoints(float) const;

		private:
			std::string name;

			std::vector<Face> faces;
			std::vector<Point3<float>> points;

			std::vector<Point2<float>> flatPointsOnYAxis(const std::vector<Point3<float>> &) const;
	};

}

#endif
