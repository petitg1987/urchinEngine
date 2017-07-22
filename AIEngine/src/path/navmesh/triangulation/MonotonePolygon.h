#ifndef ENGINE_MONOTONEPOLYGON_H
#define ENGINE_MONOTONEPOLYGON_H

#include <vector>
#include <stdexcept>
#include <map>
#include "UrchinCommon.h"

namespace urchin
{

	enum PointType
	{
		START_VERTEX, //neighbor points go down and interior angle <pi
		END_VERTEX, //neighbor points go up and interior angle <pi
		SPLIT_VERTEX, //neighbor points go down and interior angle >=pi
		MERGE_VERTEX, //neighbor points go up and interior angle >=pi
		REGULAR_DOWN_VERTEX, //previous neighbor is upper and next neighbor is lower
		REGULAR_UP_VERTEX //previous neighbor is lower and next neighbor is upper
	};

	struct TypedPoint
	{
		TypedPoint(unsigned int, PointType);

		unsigned int pointIndex;
		PointType type;
	};

	struct Edge
	{
		Edge(unsigned int, unsigned int);

		unsigned int startIndex;
		unsigned int endIndex;

		bool isProcessed;
	};

	struct EdgeHelper
	{
		EdgeHelper(Edge, unsigned int, PointType);

		Edge edge;

		unsigned int helperPointIndex;
		PointType helperPointType;
	};

	class MonotonePolygonError : public std::runtime_error
	{
		public:
			MonotonePolygonError(const std::string &message);
	};

	class MonotonePolygon
	{
		public:
			MonotonePolygon(const std::vector<Point2<float>> &, const std::vector<unsigned int> &);

			std::vector<std::vector<unsigned int>> createYMonotonePolygons();

		private:
			typedef std::multimap<unsigned int, Edge>::iterator it_diagonals;

			void createYMonotonePolygonsDiagonals();
			std::vector<TypedPoint> buildSortedTypedPoints(bool &) const;
			bool isFirstPointAboveSecond(unsigned int, unsigned int) const;

			void handleStartVertex(unsigned int);
			void handleSplitVertex(unsigned int);
			void handleEndVertex(unsigned int);
			void handleMergeVertex(unsigned int);
			void handleRegularDownVertex(unsigned int);
			void handleRegularUpVertex(unsigned int);

			unsigned int nextPointIndex(unsigned int) const;
			unsigned int previousPointIndex(unsigned int) const;

			void createEdgeHelper(unsigned int, unsigned int, PointType);
			std::vector<EdgeHelper>::iterator findEdgeHelper(unsigned int);
			std::vector<EdgeHelper>::iterator findNearestLeftEdgeHelper(unsigned int);
			void createDiagonals(unsigned int, unsigned int);

			unsigned int retrieveNextPointIndex(unsigned int, unsigned int);
			std::vector<std::pair<unsigned int, it_diagonals>> retrievePossibleNextPoints(unsigned int);
			void markDiagonalProcessed(it_diagonals);

			void logInputData(const std::string &, Logger::CriticalityLevel) const;
			void logOutputData(const std::string &, const std::vector<std::vector<unsigned int>> &, Logger::CriticalityLevel) const;

			const std::vector<Point2<float>> &polygonPoints;
			const std::vector<unsigned int> &endContourIndices; //e.g.: 'polygonPoints' contains 5 CCW points and 4 CW points (hole). So, 'endContourIndices' will have values: 5 and 9.

			std::vector<EdgeHelper> edgeHelpers;
			std::multimap<unsigned int, Edge> diagonals;
	};

}

#endif
