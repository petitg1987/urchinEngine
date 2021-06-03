#ifndef URCHINENGINE_RESIZEPOLYGON2DSERVICE_H
#define URCHINENGINE_RESIZEPOLYGON2DSERVICE_H

#include <vector>

#include <math/algebra/point/Point2.h>

namespace urchin {

    template<class T> class ResizePolygon2DService {
        public:
            ResizePolygon2DService() = delete;

            static void resizePolygon(std::vector<Point2<T>>&, T);
    };

}

#endif
