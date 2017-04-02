#ifndef ENGINE_CSGPOLYGON_H
#define ENGINE_CSGPOLYGON_H

#include <vector>
#include "UrchinCommon.h"

namespace urchin
{

	class CSGPolygon
	{
		public:
			CSGPolygon(const std::vector<Point2<float>> &);
			CSGPolygon(const CSGPolygon &);
			CSGPolygon(CSGPolygon &&);
			CSGPolygon& operator=(CSGPolygon &&);

			const std::vector<Point2<float>> &getCwPoints() const;

		private:
			std::vector<Point2<float>> cwPoints;

	};

}

#endif
