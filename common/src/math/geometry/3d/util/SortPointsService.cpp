#include <algorithm>

#include "SortPointsService.h"

namespace urchin {

    /**
     * Sort points in clockwise direction in a right hand coordinate system (Z+ directed to the observer) when the normal is directed to the observer.
     */
    template<class T> std::vector<Point3<T>> SortPointsService<T>::sortPointsClockwise(const std::vector<Point3<T>>& coplanarPoints, const Vector3<T>& normal) {
        std::vector<Point3<T>> sortedPoints;
        sortedPoints.reserve(coplanarPoints.size());

        for (std::size_t i=0; i<2 && i<coplanarPoints.size(); i++) {
            sortedPoints.push_back(coplanarPoints[i]);
        }

        for (std::size_t i=2; i<coplanarPoints.size(); ++i) {
            std::size_t newPointIndex = sortedPoints.size();
            sortedPoints.push_back(coplanarPoints[i]);
            while (newPointIndex>1) {
                if (isNewPointClockwiseSorted(sortedPoints, normal, newPointIndex)) {
                    break;
                }

                std::swap(sortedPoints[newPointIndex], sortedPoints[newPointIndex-1]);
                --newPointIndex;
            }
        }

        return sortedPoints;
    }

    /**
     * Sort points in counter clockwise direction in a right hand coordinate system (Z+ directed to the observer) when the normal is directed to the observer.
     */
    template<class T> std::vector<Point3<T>> SortPointsService<T>::sortPointsCounterClockwise(const std::vector<Point3<T>>& coplanarPoints, const Vector3<T>& normal) {
        return sortPointsClockwise(coplanarPoints, -normal);
    }

    template<class T> bool SortPointsService<T>::isNewPointClockwiseSorted(const std::vector<Point3<T>>& points, const Vector3<T>& normal, unsigned int newPointIndex) {
        for (unsigned int i=0; i<3; ++i) { //3 (counter-)clockwise tests: previous point, previous point, new point AND previous point, new point, next point AND new point, next point, next point
            unsigned int firstPointIndex = (newPointIndex-2 + i)%points.size();
            unsigned int secondPointIndex = (newPointIndex-1 + i)%points.size();
            unsigned int thirdPointIndex = (newPointIndex-0 + i)%points.size();

            Vector3<T> vectorFirstSecond = points[firstPointIndex].vector(points[secondPointIndex]);
            Vector3<T> vectorSecondThird = points[secondPointIndex].vector(points[thirdPointIndex]);
            if (normal.dotProduct(vectorFirstSecond.crossProduct(vectorSecondThird)) > 0.0f) { //wrong direction
                return false;
            }
        }

        return true;
    }

    //explicit template
    template class SortPointsService<float>;

    template class SortPointsService<double>;

}
