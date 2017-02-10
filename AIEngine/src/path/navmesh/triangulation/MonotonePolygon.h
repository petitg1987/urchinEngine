#ifndef ENGINE_MONOTONEPOLYGON_H
#define ENGINE_MONOTONEPOLYGON_H

#include <vector>
#include <queue>
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
		unsigned int pointIndex;
		PointType type;
	};

	class TypedPointCmp
	{
		public:
			TypedPointCmp(const std::vector<Point2<float>> &);

			bool operator()(const TypedPoint &, const TypedPoint &) const;

		private:
			const std::vector<Point2<float>> &ccwPolygonPoints;
	};

	struct Edge
	{
		unsigned int startIndex;
		unsigned int endIndex;

		bool isProcessed;
	};

	struct EdgeHelper
	{
		Edge edge;

		unsigned int helperPointIndex;
		PointType helperPointType;
	};

	typedef std::priority_queue<TypedPoint, std::vector<TypedPoint>, TypedPointCmp> typed_points_queue;

	class MonotonePolygon
	{
		public:
			MonotonePolygon(const std::vector<Point2<float>> &);

			std::vector<std::vector<Point2<float>>> createYMonotonePolygons();

		private:
			typedef std::multimap<unsigned int, Edge>::iterator ItDiagonals;

			void createYMonotonePolygonsDiagonals();
			typed_points_queue buildTypedPointsQueue() const;

			void handleStartVertex(unsigned int);
			void handleSplitVertex(unsigned int);
			void handleEndVertex(unsigned int);
			void handleMergeVertex(unsigned int);
			void handleRegularDownVertex(unsigned int);
			void handleRegularUpVertex(unsigned int);

			void createEdgeHelper(unsigned int, unsigned int, PointType);
			std::vector<EdgeHelper>::const_iterator findEdgeHelper(unsigned int) const;
			std::vector<EdgeHelper>::iterator findNearestLeftEdgeHelper(unsigned int);
			void createDiagonals(unsigned int, unsigned int);

			unsigned int retrieveNextPointIndex(unsigned int, unsigned int);
			std::vector<std::pair<int, ItDiagonals>> retrievePossibleNextPoints(unsigned int);
			void markDiagonalProcessed(ItDiagonals);

			void logImpossibleToClosePolygon() const;

			const std::vector<Point2<float>> &ccwPolygonPoints;

			std::vector<EdgeHelper> edgeHelpers;
			std::multimap<unsigned int, Edge> diagonals;
	};

}

#endif
