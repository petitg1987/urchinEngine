#ifndef ENGINE_CSGPOLYGON_H
#define ENGINE_CSGPOLYGON_H

#include <string>
#include <vector>
#include "UrchinCommon.h"

namespace urchin
{

	class CSGPolygon
	{
		public:
			CSGPolygon(std::string name, const std::vector<Point2<float>> &);
			CSGPolygon(const CSGPolygon &);
			CSGPolygon(CSGPolygon &&);
			CSGPolygon& operator=(CSGPolygon &&);

			const std::string &getName() const;

			const std::vector<Point2<float>> &getCwPoints() const;

			CSGPolygon expand(float) const;

		private:
			std::string name;
			std::vector<Point2<float>> cwPoints;

	};

	std::ostream& operator <<(std::ostream &, const CSGPolygon &);

}

#endif
