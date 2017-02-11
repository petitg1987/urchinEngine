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

		private:
			const std::vector<Point2<float>> &ccwPolygonPoints;
	};

	class Triangulation
	{
		public:
			Triangulation(const std::vector<Point2<float>> &);

			void triangulate() const;

		private:
			void triangulateMonotonePolygon(const std::vector<Point2<float>> &) const;
			std::vector<SidedPoint> buildSidedPointsVector(const std::vector<Point2<float>> &) const;

			std::vector<Point2<float>> ccwPolygonPoints;
	};

}

#endif
