#ifndef ENGINE_NAVPOLYGON_H
#define ENGINE_NAVPOLYGON_H

#include <vector>
#include "UrchinCommon.h"

namespace urchin
{

	class NavPolygon
	{
		public:
			NavPolygon();
			NavPolygon(const std::vector<Point3<float>> &);

			unsigned int addPoint(const Point3<float> &);
			const std::vector<Point3<float>> &getPoints() const;
			const Point3<float> &getPoint(unsigned int) const;

		private:
			std::vector<Point3<float>> points;

			//links

			//bool crouch
			//bool water

	};

}

#endif
