#ifndef ENGINE_POLYGONSSUBTRACTION_H
#define ENGINE_POLYGONSSUBTRACTION_H

#include <vector>
#include "UrchinCommon.h"

#include "CSGPolygon.h"

namespace urchin
{

    template<class T> struct SubtractionPoint
    {
        Point2<T> point;
        bool isOutside;
        bool isProcessed;
        int crossPointIndex; //-1 if not exist.
    };

    template<class T> struct IntersectionPoint
    {
        IntersectionPoint(const Point2<T> &, int);

        Point2<T> intersectionPoint;
        int crossPointIndex;
    };

    template<class T> class PolygonsSubtraction : public Singleton<PolygonsSubtraction<T>>
    {
        public:
            friend class Singleton<PolygonsSubtraction<T>>;

            std::vector<CSGPolygon<T>> subtractPolygons(const CSGPolygon<T> &, const CSGPolygon<T> &) const;

        private:
            PolygonsSubtraction();
            virtual ~PolygonsSubtraction();

            void buildSubtractionPointsFor(const CSGPolygon<T> &, std::map<unsigned int, std::vector<SubtractionPoint>> &,
                                           const CSGPolygon<T> &, std::map<unsigned int, std::vector<SubtractionPoint>> &) const;
            void buildIntersectionPoints(const CSGPolygon<T> &, std::map<unsigned int, std::vector<IntersectionPoint>> &,
                                         const CSGPolygon<T> &, std::map<unsigned int, std::vector<IntersectionPoint>> &) const;
            Point2<T> determineMiddlePoint(const Point2<T> &, const Point2<T> &) const;
            bool pointInsidePolygon(const CSGPolygon<T> &, const Point2<T> &) const;
    };

}

#endif
