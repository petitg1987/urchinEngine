#include <cassert>

#include "ResizeConvexHull2DService.h"
#include "math/geometry/2d/util/ResizePolygon2DService.h"
#include "math/algebra/point/Point2.h"
#include "math/algebra/vector/Vector2.h"

namespace urchin {

    template<class T> ResizeConvexHull2DService<T>::ResizeConvexHull2DService() :
            Singleton<ResizeConvexHull2DService<T>>() {

    }

    /**
     * @param distance All edge of convex hull shape will be moved along their normal to the specified distance.
     * Positive distance will extend convex hull shape. Negative distance are not supported by this algorithm.
     */
    template<class T> std::unique_ptr<ConvexHullShape2D<T>> ResizeConvexHull2DService<T>::resizeConvexHullShape(const ConvexHullShape2D<T>& originalConvexHullShape, T distance) const {
        assert(distance > (0.0 - std::numeric_limits<T>::epsilon()));

        std::vector<Point2<T>> ccwPoints = originalConvexHullShape.getPoints();
        ResizePolygon2DService<T>::instance()->resizePolygon(ccwPoints, distance);

        return ConvexHullShape2D<T>::createFromCcwConvexPoints(ccwPoints);
    }

    template<class T> std::unique_ptr<ConvexHull2D<T>> ResizeConvexHull2DService<T>::resizeConvexHull(const ConvexHull2D<T>& originalConvexHull, T distance) const {
        std::unique_ptr<ConvexHullShape2D<T>> convexHullShapeResized = resizeConvexHullShape(originalConvexHull.localizedConvexHullShape, distance);

        return ConvexHull2D<T>::createFromCcwConvexPoints(convexHullShapeResized->getPoints());
    }

    //explicit template
    template class ResizeConvexHull2DService<float>;

    template class ResizeConvexHull2DService<double>;

}
