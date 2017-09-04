#ifndef URCHINENGINE_TRIANGULATIONALGORITHM_H
#define URCHINENGINE_TRIANGULATIONALGORITHM_H

#include <vector>
#include <map>
#include "UrchinCommon.h"

#include "path/navmesh/triangulation/IndexedTriangleMesh.h"
#include "path/navmesh/triangulation/MonotonePolygon.h"

namespace urchin
{

	struct SidedPoint
	{
		SidedPoint(unsigned int, bool);

		unsigned int pointIndex;
		bool onLeft; //indicate if point is on left of monotone polygon or right
	};

	struct TriangleEdge
	{
		TriangleEdge(unsigned int, unsigned int);

		unsigned int triangleIndex;
		unsigned int edgeIndex;
	};

	class TriangulationAlgorithm
	{
		public:
			explicit TriangulationAlgorithm(const std::vector<Point2<float>> &);

			std::vector<Point2<float>> getPolygonPoints() const;

			unsigned int addHolePoints(const std::vector<Point2<float>> &);
			unsigned int getHolesSize() const;
			std::vector<Point2<float>> getHolePoints(unsigned int) const;

			unsigned int getAllPointsSize() const;

			std::vector<IndexedTriangleMesh> triangulate();

		private:
			std::vector<IndexedTriangleMesh> triangulateMonotonePolygon(const MonotonePolygon &);
			std::vector<SidedPoint> buildSortedSidedPoints(const std::vector<unsigned int> &) const;
			bool isFirstPointAboveSecond(unsigned int, unsigned int) const;

			void determineNeighbors(std::vector<IndexedTriangleMesh> &, const MonotonePolygon &);
			void determineNeighborsInsideMonotone(std::vector<IndexedTriangleMesh> &);
			void determineNeighborsBetweenMonotones(std::vector<IndexedTriangleMesh> &, const MonotonePolygon &);
			bool areSameEdge(const IndexedTriangleMesh &, unsigned int, unsigned int, const IndexedTriangleMesh &, unsigned int, unsigned int) const;
			uint_fast64_t computeEdgeId(unsigned int, unsigned int) const;

			void logOutputData(const std::string &, const std::vector<IndexedTriangleMesh> &, Logger::CriticalityLevel) const;

			std::vector<Point2<float>> polygonPoints;
			std::vector<unsigned int> endContourIndices; //e.g.: 'polygonPoints' contains 5 CCW points and 4 CW points (hole). So, 'endContourIndices' will have values: 5 and 9.

			std::vector<IndexedTriangleMesh> triangles;
			int missingTriangleNeighbor;
			std::map<uint_fast64_t, TriangleEdge> sharedMonotoneEdges;
	};

}

#endif
