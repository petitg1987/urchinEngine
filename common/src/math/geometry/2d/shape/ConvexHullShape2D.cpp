#include <algorithm>
#include <cmath>

#include <math/geometry/2d/shape/ConvexHullShape2D.h>
#include <math/geometry/2d/util/ResizeConvexHull2DService.h>
#include <math/geometry/2d/Line2D.h>

namespace urchin {

    /**
     * @param points Points used to construct the convex hull shape. Points inside the convex hull shape are accepted but will unused.
     * Duplicate points are supported.
     */
    template<class T> ConvexHullShape2D<T>::ConvexHullShape2D(const std::vector<Point2<T>>& points) {
        //Monotone Chain algorithm
        std::size_t nbPoints = points.size();
        unsigned int k = 0; //number of points in convex hull

        convexHullPoints.resize(2 * nbPoints);

        //sort points lexicographically
        std::vector<Point2<T>> sortedPoints;
        sortedPoints.resize(nbPoints);
        std::ranges::copy(points, sortedPoints.begin());
        std::sort(sortedPoints.begin(), sortedPoints.end());

        //build lower convex hull
        for (std::size_t i = 0; i < nbPoints; i++) {
            while (k >= 2 && Line2D<T>(convexHullPoints[k-2], convexHullPoints[k-1]).ccw(sortedPoints[i]) <= 0.0) { //clockwise detected, we remove the point
                k--;
            }

            if (k == 0 || convexHullPoints[k-1] != sortedPoints[i]) {
                convexHullPoints[k++] = sortedPoints[i];
            }
        }

        //build upper convex hull
        unsigned int t = k + 1;
        for (int i = (int)nbPoints - 2; i >= 0; i--) {
            while (k >= t && Line2D<T>(convexHullPoints[k - 2], convexHullPoints[k-1]).ccw(sortedPoints[(std::size_t)i]) <= 0.0) { //clockwise detected, we remove the point
                k--;
            }

            if (k == 0 || convexHullPoints[k-1] != sortedPoints[(std::size_t)i]) {
                convexHullPoints[k++] = sortedPoints[(std::size_t)i];
            }
        }

        convexHullPoints.resize((unsigned long)(std::max(((int)k) - 1, 0))); //k-1: remove the last point which is the same that the first point of lower list
    }

    template<class T> std::unique_ptr<ConvexHullShape2D<T>> ConvexHullShape2D<T>::createFromCcwConvexPoints(const std::vector<Point2<T>>& ccwConvexPoints) {
        std::unique_ptr<ConvexHullShape2D<T>> convexHullShape = std::make_unique<ConvexHullShape2D<T>>();
        convexHullShape->convexHullPoints = ccwConvexPoints;

        return convexHullShape;
    }

    /**
     * @return Points of the convex hull shape in counter clockwise direction
     */
    template<class T> const std::vector<Point2<T>> &ConvexHullShape2D<T>::getPoints() const {
        return convexHullPoints;
    }

    template<class T> Point2<T> ConvexHullShape2D<T>::getSupportPoint(const Vector2<T>& direction) const {
        T maxPointDotDirection = Point2<T>(0.0, 0.0).vector(convexHullPoints[0]).dotProduct(direction);
        Point2<T> maxPoint = convexHullPoints[0];

        for (std::size_t i = 0; i < convexHullPoints.size(); ++i) {
            T currentPointDotDirection = Point2<T>(0.0, 0.0).vector(convexHullPoints[i]).dotProduct(direction);
            if (currentPointDotDirection > maxPointDotDirection) {
                maxPointDotDirection = currentPointDotDirection;
                maxPoint = convexHullPoints[i];
            }
        }

        return maxPoint;
    }

    template<class T> T ConvexHullShape2D<T>::area() const {
        T area = 0.0;
        std::size_t j;

        for (std::size_t i = 0; i < convexHullPoints.size(); i++) {
            j = (i + 1) % convexHullPoints.size();
            area += convexHullPoints[i].X * convexHullPoints[j].Y;
            area -= convexHullPoints[i].Y * convexHullPoints[j].X;
        }

        area /= (T)2.0;
        return std::fabs(area);
    }

    /**
     * @param distance All edge of convex hull shape will be moved along their normal to the specified distance.
     * Positive distance will extend convex hull shape. Negative distance are not supported by this algorithm.
     */
    template<class T> std::unique_ptr<ConvexHullShape2D<T>> ConvexHullShape2D<T>::resize(T distance) const {
        return ResizeConvexHull2DService<T>::resizeConvexHullShape(*this, distance);
    }

    //explicit template
    template class ConvexHullShape2D<float>;

    template class ConvexHullShape2D<double>;

}
