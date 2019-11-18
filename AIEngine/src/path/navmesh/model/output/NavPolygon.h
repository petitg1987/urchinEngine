#ifndef URCHINENGINE_NAVPOLYGON_H
#define URCHINENGINE_NAVPOLYGON_H

#include <vector>
#include "UrchinCommon.h"

#include "path/navmesh/model/output/NavTriangle.h"
#include "path/navmesh/model/output/topography/NavTopography.h"

namespace urchin
{

	class NavPolygon
	{
		public:
			NavPolygon(std::string, std::vector<Point3<float>> &&, const NavTopography *);
			NavPolygon(const NavPolygon &);
			~NavPolygon();

			const std::string &getName() const;

			const std::vector<Point3<float>> &getPoints() const;
			const Point3<float> &getPoint(unsigned int) const;

			void addTriangles(const std::vector<std::shared_ptr<NavTriangle>> &, const std::shared_ptr<NavPolygon> &);
			const std::vector<std::shared_ptr<NavTriangle>> &getTriangles() const;
			const std::shared_ptr<NavTriangle> &getTriangle(std::size_t) const;

            const NavTopography *getNavTopography() const;

		private:
			std::string name;

			std::vector<Point3<float>> points;
			std::vector<std::shared_ptr<NavTriangle>> triangles;

            const NavTopography *navTopography;
	};

}

#endif
