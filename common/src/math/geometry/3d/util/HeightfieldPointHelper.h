#ifndef URCHINENGINE_HEIGHTFIELDPOINTHELPER_H
#define URCHINENGINE_HEIGHTFIELDPOINTHELPER_H

#include <vector>

#include "math/algebra/point/Point2.h"
#include "math/algebra/point/Point3.h"
#include "math/geometry/2d/object/LineSegment2D.h"

namespace urchin {

    template <class T> class DistanceToStartPointComp {
        public:
            explicit DistanceToStartPointComp(const Point3<T>&);
            bool operator() (const Point3<T>&, const Point3<T>&) const;

        private:
            Point3<T> startPoint;
    };

    template<class T> class HeightfieldPointHelper {
        public:
            static const T PARALLEL_EPSILON;

            HeightfieldPointHelper(const std::vector<Point3<T>>&, unsigned int);

            Point3<T> findPointAt(const Point2<T>&) const;
            T findHeightAt(const Point2<T>&) const;
            std::vector<Point3<T>> followTopography(const Point3<T>&, const Point3<T>&) const;

        private:
            void addIntersectionPoint(const LineSegment2D<T>&, const LineSegment2D<T>&, std::vector<Point3<T>>&) const;
            bool isParallelToXAxis(const LineSegment2D<T>&, T) const;
            bool isParallelToZAxis(const LineSegment2D<T>&, T) const;

            const std::vector<Point3<T>> &heightfieldPoints;

            unsigned int heightfieldXSize;
            unsigned int heightfieldZSize;

            T xInterval;
            T zInterval;
    };

}

#endif
