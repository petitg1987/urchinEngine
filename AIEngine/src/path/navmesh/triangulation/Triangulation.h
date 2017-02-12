#ifndef ENGINE_TRIANGULATION_H
#define ENGINE_TRIANGULATION_H

#include <vector>
#include "UrchinCommon.h"

namespace urchin
{

	struct SidedPoint
	{
		unsigned int pointIndex;
		bool onLeft; //indicate if point is on left of monotone polygon or right
	};

	class Triangulation
	{
		public:
			Triangulation(const std::vector<Point2<float>> &);

			const std::vector<Point2<float>> &getPolygonPoints() const;

			std::vector<IndexedTriangle2D<float>> triangulate() const;

		private:
			void triangulateMonotonePolygon(const std::vector<unsigned int> &, std::vector<IndexedTriangle2D<float>> &) const;
			std::vector<SidedPoint> buildSortedSidedPoints(const std::vector<unsigned int> &) const;
			bool isFirstPointAboveSecond(unsigned int, unsigned int) const;

			std::vector<Point2<float>> ccwPolygonPoints;
	};

}

#endif
