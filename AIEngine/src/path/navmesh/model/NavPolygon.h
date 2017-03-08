#ifndef ENGINE_NAVPOLYGON_H
#define ENGINE_NAVPOLYGON_H

#include <vector>
#include "UrchinCommon.h"

namespace urchin
{

	class NavPolygon
	{
		public:
			NavPolygon(const std::vector<Point3<float>> &, const std::vector<IndexedTriangle2D<float>> &);

			const std::vector<Point3<float>> &getPoints() const;
			const Point3<float> &getPoint(unsigned int) const;

			const std::vector<IndexedTriangle2D<float>> &getTriangles() const;
			const IndexedTriangle2D<float> &getTriangle(unsigned int) const;

		private:
			std::vector<Point3<float>> points;
			std::vector<IndexedTriangle2D<float>> triangles;

			//links

			//bool crouch
			//bool water

	};

}

#endif
