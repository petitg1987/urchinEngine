#ifndef URCHINENGINE_SORTPOINTSSERVICE_H
#define URCHINENGINE_SORTPOINTSSERVICE_H

#include <vector>

#include <math/algebra/point/Point3.h>
#include <math/algebra/vector/Vector3.h>

namespace urchin {

    template<class T> class SortPointsService {
        public:
            SortPointsService() = delete;

            static std::vector<Point3<T>> sortPointsClockwise(const std::vector<Point3<T>>&, const Vector3<T>&);
            static std::vector<Point3<T>> sortPointsCounterClockwise(const std::vector<Point3<T>>&, const Vector3<T>&);

        private:
            static bool isNewPointClockwiseSorted(const std::vector<Point3<T>>&, const Vector3<T>&, unsigned int);
    };

}

#endif
