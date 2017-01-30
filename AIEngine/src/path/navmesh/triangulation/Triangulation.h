#ifndef ENGINE_TRIANGULATION_H
#define ENGINE_TRIANGULATION_H

#include <vector>
#include <queue>
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

	struct EdgeHelper
	{
		unsigned int edgeStartIndex; //edgeEndIndex=edgeStartIndex+1

		unsigned int helperPointIndex;
		PointType helperPointType;
	};

	typedef std::priority_queue<TypedPoint, std::vector<TypedPoint>, TypedPointCmp> typed_points_queue;

	class Triangulation
	{
		public:
			Triangulation(const std::vector<Point2<float>> &);

			void triangulate() const;

		private:
			void createYMonotonePolygons() const;
			typed_points_queue buildTypedPointsQueue() const;

			void handleStartVertex(unsigned int, std::vector<EdgeHelper> &) const;
			void handleSplitVertex(unsigned int, std::vector<EdgeHelper> &) const;
			void handleEndVertex(unsigned int, std::vector<EdgeHelper> &) const;
			void handleMergeVertex(unsigned int, std::vector<EdgeHelper> &) const;
			void handleRegularDownVertex(unsigned int, std::vector<EdgeHelper> &) const;
			void handleRegularUpVertex(unsigned int, std::vector<EdgeHelper> &) const;

			std::vector<EdgeHelper>::iterator findEdgeHelper(unsigned int, std::vector<EdgeHelper> &) const;
			std::vector<EdgeHelper>::iterator findNearestLeftEdgeHelper(unsigned int, std::vector<EdgeHelper> &) const;

			std::vector<Point2<float>> ccwPolygonPoints;
	};

}

#endif
