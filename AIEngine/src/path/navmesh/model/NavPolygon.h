#ifndef URCHINENGINE_NAVPOLYGON_H
#define URCHINENGINE_NAVPOLYGON_H

#include <vector>
#include "UrchinCommon.h"

#include "path/navmesh/model/NavTriangle.h"

namespace urchin
{

	class NavPolygon
	{
		public:
			NavPolygon(std::string, const std::vector<Point3<float>> &, const std::vector<NavTriangle> &);
			NavPolygon(const NavPolygon &);

			const std::string &getName() const;

			const std::vector<Point3<float>> &getPoints() const;
			const Point3<float> &getPoint(unsigned int) const;

			const std::vector<NavTriangle> &getTriangles() const;
			const NavTriangle &getTriangle(unsigned int) const;

		private:
			std::string name;

			std::vector<Point3<float>> points;
			std::vector<NavTriangle> triangles;
	};

}

#endif
