#ifndef ENGINE_TRIANGULATION_H
#define ENGINE_TRIANGULATION_H

#include <vector>
#include <queue>
#include "UrchinCommon.h"

namespace urchin
{
	class Triangulation
	{
		public:
			Triangulation(const std::vector<Point2<float>> &);

			void triangulate() const;

		private:
			std::vector<Point2<float>> ccwPolygonPoints;
	};

}

#endif
