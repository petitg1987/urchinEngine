#ifndef URCHINENGINE_FINDPOINTSERVICE_H
#define URCHINENGINE_FINDPOINTSERVICE_H

#include <vector>

#include "pattern/singleton/Singleton.h"
#include "math/algebra/point/Point2.h"
#include "math/algebra/point/Point3.h"

namespace urchin
{

    template<class T> class FindPointService : public Singleton<FindPointService<T>>
    {
        public:
            friend class Singleton<FindPointService<T>>;

            Point3<T> findPointAt(const Point2<T> &, const std::vector<Point3<T>> &, unsigned int) const;
            T findHeightAt(const Point2<T> &, const std::vector<Point3<T>> &, unsigned int) const;

        private:
            FindPointService() = default;
            virtual ~FindPointService() = default;
    };

}

#endif
