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
			const std::vector<Point2<float>> &getCwFootprintPoints() const;

		private:
			std::string name;

			std::vector<Face> faces;
			std::vector<Point3<float>> points;
			std::unique_ptr<ConvexHull2D<float>> footprintConvexHull;

			std::vector<Point2<float>> flatPointsOnYAxis(const std::vector<Point3<float>> &);

	};

}

#endif
