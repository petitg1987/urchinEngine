#ifndef ENGINE_TRIANGULATION_H
#define ENGINE_TRIANGULATION_H

#include <vector>
#include "UrchinCommon.h"

namespace urchin
{

	struct SidedPoint
	{
		SidedPoint(unsigned int, bool);

		unsigned int pointIndex;
		bool onLeft; //indicate if point is on left of monotone polygon or right
	};

	class Triangulation
	{
		public:
			explicit Triangulation(const std::vector<Point2<float>> &);

			std::vector<Point2<float>> getPolygonPoints() const;

			unsigned int addHolePoints(const std::vector<Point2<float>> &);
			unsigned int getHolesSize() const;
			std::vector<Point2<float>> getHolePoints(unsigned int) const;

			unsigned int getAllPointsSize() const;

			std::vector<IndexedTriangle2D<float>> triangulate() const;

		private:
			void triangulateMonotonePolygon(const std::vector<unsigned int> &, std::vector<IndexedTriangle2D<float>> &) const;
			std::vector<SidedPoint> buildSortedSidedPoints(const std::vector<unsigned int> &) const;
			bool isFirstPointAboveSecond(unsigned int, unsigned int) const;

			void logOutputData(const std::string &, const std::vector<IndexedTriangle2D<float>> &, Logger::CriticalityLevel) const;

			std::vector<Point2<float>> polygonPoints;
			std::vector<unsigned int> endContourIndices; //e.g.: 'polygonPoints' contains 5 CCW points and 4 CW points (hole). So, 'endContourIndices' will have values: 5 and 9.
	};

}

#endif
