#pragma once

#include <memory>
#include <vector>

#include <math/geometry/2d/shape/ConvexHullShape2D.h>
#include <math/geometry/2d/object/ConvexHull2D.h>

namespace urchin {

    template<class T> class ResizeConvexHull2DService {
        public:
            ResizeConvexHull2DService() = delete;

            static std::unique_ptr<ConvexHullShape2D<T>> resizeConvexHullShape(const ConvexHullShape2D<T>&, T);
            static std::unique_ptr<ConvexHull2D<T>> resizeConvexHull(const ConvexHull2D<T>&, T);
    };

}
