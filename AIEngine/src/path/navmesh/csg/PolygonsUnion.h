#ifndef ENGINE_POLYGONSUNION_H
#define ENGINE_POLYGONSUNION_H

#include <vector>
#include "UrchinCommon.h"

#include "CSGPolygon.h"

namespace urchin
{

	template<class T> struct CSGIntersection
	{
		bool hasIntersection;
		Point2<T> intersectionPoint;
		uint_fast64_t intersectionId;
		unsigned int edgeStartPointIndex;
		unsigned int edgeEndPointIndex;
	};

	template<class T> class PolygonsUnion : public Singleton<PolygonsUnion<T>>
	{
		public:
			friend class Singleton<PolygonsUnion<T>>;

			std::vector<CSGPolygon<T>> unionPolygons(const std::vector<CSGPolygon<T>> &) const;

		private:
			PolygonsUnion();
			virtual ~PolygonsUnion();

			std::vector<CSGPolygon<T>> unionTwoPolygons(const CSGPolygon<T> &, const CSGPolygon<T> &) const;

			unsigned int findStartPoint(const CSGPolygon<T> &, const CSGPolygon<T> &, const CSGPolygon<T> *&) const;
			unsigned int findLowestPointIndex(const CSGPolygon<T> &) const;
			CSGIntersection<T> findFirstIntersectionOnEdge(const LineSegment2D<T> &, const Point2<T> &, const CSGPolygon<T> *, const CSGPolygon<T> *, unsigned int, std::set<uint_fast64_t> &) const;
			bool isIntersectionAngleBetter(const LineSegment2D<T> &, const LineSegment2D<T> &) const;
			bool isIntersectionAngleBetter(const Point2<T> &, const LineSegment2D<T> &, const Point2<T> &) const;
            uint_fast64_t getIntersectionId(const CSGPolygon<T> *, unsigned int, const CSGPolygon<T> *, unsigned int) const;

			bool areSamePoints(const Point2<T> &, const Point2<T> &) const;
			bool pointInsideOrOnPolygon(const CSGPolygon<T> *, const Point2<T> &) const;

			void logInputData(const CSGPolygon<T> &, const CSGPolygon<T> &, const std::string &, Logger::CriticalityLevel) const;
	};

}

#endif
