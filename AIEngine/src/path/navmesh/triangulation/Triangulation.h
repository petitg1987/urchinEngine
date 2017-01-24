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
		REGULAR_VERTEX //one neighbor go up and the other go down
	};

	struct TypedPoint
	{
		Point2<float> point;
		PointType type;

		bool operator()(const TypedPoint &left, const TypedPoint &right) const;
	};

	typedef std::priority_queue<TypedPoint, std::vector<TypedPoint>, TypedPoint> typed_points_queue;

	class Triangulation
	{
		public:
			Triangulation(const std::vector<Point2<float>> &);

			void triangulate() const;

		private:
			void createYMonotonePolygons() const;
			typed_points_queue buildTypedPointsQueue() const;

			std::vector<Point2<float>> ccwPolygonPoints;
	};



}

#endif
