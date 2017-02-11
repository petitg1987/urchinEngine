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

	class SidedPointCmp
	{
		public:
			SidedPointCmp(const std::vector<Point2<float>> &);

			bool operator()(const SidedPoint &, const SidedPoint &) const;
			bool isAbove(unsigned int, unsigned int) const;

		private:
			const std::vector<Point2<float>> &ccwPolygonPoints;
	};

	class Triangulation
	{
		public:
			Triangulation(const std::vector<Point2<float>> &);

			std::vector<IndexedTriangle2D<float>> triangulate() const;

		private:
			void triangulateMonotonePolygon(const std::vector<unsigned int> &, std::vector<IndexedTriangle2D<float>> &) const;
			std::vector<SidedPoint> buildSidedPointsVector(const std::vector<unsigned int> &) const;

			std::vector<Point2<float>> ccwPolygonPoints;
	};

}

#endif
