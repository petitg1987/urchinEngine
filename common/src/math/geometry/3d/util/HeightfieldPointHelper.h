#ifndef URCHINENGINE_HEIGHTFIELDPOINTHELPER_H
#define URCHINENGINE_HEIGHTFIELDPOINTHELPER_H

#include <vector>

#include "pattern/singleton/Singleton.h"
#include "math/algebra/point/Point2.h"
#include "math/algebra/point/Point3.h"

namespace urchin
{

    template<class T> class HeightfieldPointHelper
    {
        public:
            HeightfieldPointHelper(const std::vector<Point3<T>> &, unsigned int);

            Point3<T> findPointAt(const Point2<T> &) const;
            T findHeightAt(const Point2<T> &) const;
            std::vector<Point3<T>> establishPath(const Point3<T> &, const Point3<T> &) const;

        private:
            const std::vector<Point3<T>> &heightfieldPoints;

            unsigned int heightfieldXSize;
            unsigned int heightfieldZSize;

            T xInterval;
            T zInterval;
    };

}

#endif
