#include <limits>
#include <algorithm>

#include <math/geometry/3d/util/ResizeConvexHull3DService.h>

namespace urchin {

    /**
     * @param distance All planes of convex hull shape will be moved along their normal to the specified distance.
     * Positive distance will extend convex hull shape and negative distance will shrink the convex hull shape.
     * @return Convex hull shape resized.
     */
    template<class T> std::unique_ptr<ConvexHullShape3D<T>> ResizeConvexHull3DService<T>::resizeConvexHullShape(const ConvexHullShape3D<T>& originalConvexHullShape, T distance) {
         std::map<std::size_t, Plane<T>> planes = buildPlanesFromConvexHullShape(originalConvexHullShape);
        shiftPlanes(planes, distance);

        return resizeConvexHullShape(originalConvexHullShape, planes);
    }

    template<class T> std::unique_ptr<ConvexHull3D<T>> ResizeConvexHull3DService<T>::resizeConvexHull(const ConvexHull3D<T>& originalConvexHull, T distance) {
        std::unique_ptr<ConvexHullShape3D<T>> convexHullShapeResized = resizeConvexHullShape(originalConvexHull.localizedConvexHullShape, distance);
        return std::make_unique<ConvexHull3D<T>>(*convexHullShapeResized);
    }

    template<class T> std::unique_ptr<ConvexHullShape3D<T>> ResizeConvexHull3DService<T>::resizeConvexHullShape(const ConvexHullShape3D<T>& originalConvexHullShape, const std::map<std::size_t, Plane<T>>& expandedPlanes) {
        bool containUselessPoints = false;
        std::map<std::size_t, ConvexHullPoint<T>> newConvexHullPoints;
        for (const auto& itPoint : originalConvexHullShape.getConvexHullPoints()) {
            std::vector<Plane<T>> threePlanes = findThreeNonParallelPlanes(itPoint.second.triangleIndices, expandedPlanes);
            if (threePlanes.size() == 3) {
                Vector3<T> n1CrossN2 = threePlanes[0].getNormal().crossProduct(threePlanes[1].getNormal());
                Vector3<T> n2CrossN3 = threePlanes[1].getNormal().crossProduct(threePlanes[2].getNormal());
                Vector3<T> n3CrossN1 = threePlanes[2].getNormal().crossProduct(threePlanes[0].getNormal());

                Point3<T> newPoint = Point3<T>(n2CrossN3 * threePlanes[0].getDistanceToOrigin());
                newPoint += Point3<T>(n3CrossN1 * threePlanes[1].getDistanceToOrigin());
                newPoint += Point3<T>(n1CrossN2 * threePlanes[2].getDistanceToOrigin());
                newPoint *= -(T)1.0 / threePlanes[0].getNormal().dotProduct(n2CrossN3);

                ConvexHullPoint<T> convexHullPoint;
                convexHullPoint.point = newPoint;
                convexHullPoint.triangleIndices = itPoint.second.triangleIndices;
                newConvexHullPoints.emplace(itPoint.first, convexHullPoint);
            } else { //useless point found on convex hull (could be removed from convex hull without impact)
                containUselessPoints = true;
            }
        }

        if (containUselessPoints) { //re-build convex hull without useless points
            std::vector<Point3<T>> vNewConvexHullPoints;
            vNewConvexHullPoints.reserve(newConvexHullPoints.size());
            for (const auto& newConvexHullPoint : newConvexHullPoints) {
                vNewConvexHullPoints.push_back(newConvexHullPoint.second.point);
            }

            return std::make_unique<ConvexHullShape3D<T>>(vNewConvexHullPoints);
        }

        return std::make_unique<ConvexHullShape3D<T>>(newConvexHullPoints, originalConvexHullShape.getIndexedTriangles());
    }

    template<class T> std::unique_ptr<ConvexHull3D<T>> ResizeConvexHull3DService<T>::resizeConvexHull(const ConvexHull3D<T>& originalConvexHull, const std::map<std::size_t, Plane<T>>& expandedPlanes) {
        std::unique_ptr<ConvexHullShape3D<T>> convexHullShapeResized = resizeConvexHullShape(originalConvexHull.localizedConvexHullShape, expandedPlanes);
        return std::make_unique<ConvexHull3D<T>>(*convexHullShapeResized);
    }

    template<class T> std::map<std::size_t, Plane<T>> ResizeConvexHull3DService<T>::buildPlanesFromConvexHullShape(const ConvexHullShape3D<T>& convexHull) {
        std::map<std::size_t, Plane<T>> planes;

        for (const auto& itTriangles : convexHull.getIndexedTriangles()) {
            const IndexedTriangle3D<T>& indexedTriangle = itTriangles.second;
            const Point3<T>& point1 = convexHull.getConvexHullPoints().at(indexedTriangle.getIndex(0)).point;
            const Point3<T>& point2 = convexHull.getConvexHullPoints().at(indexedTriangle.getIndex(1)).point;
            const Point3<T>& point3 = convexHull.getConvexHullPoints().at(indexedTriangle.getIndex(2)).point;

            planes.emplace(itTriangles.first, Plane<T>(point1, point2, point3)); //plane is built with normal outside convex hull
        }

        return planes;
    }

    template<class T> void ResizeConvexHull3DService<T>::shiftPlanes(std::map<std::size_t, Plane<T>>& planes, T distance) {
        for (auto& itPlanes : planes) {
            itPlanes.second.setDistanceToOrigin(itPlanes.second.getDistanceToOrigin() - distance);
        }
    }

    template<class T> std::vector<Plane<T>> ResizeConvexHull3DService<T>::findThreeNonParallelPlanes(const std::vector<std::size_t>& planeIndices, const std::map<std::size_t, Plane<T>>& allPlanes) {
        constexpr float PARALLEL_COMPARISON_TOLERANCE = 0.01f;

        std::vector<Plane<T>> nonParallelPlanes;
        nonParallelPlanes.reserve(3);

        const Plane<T>& plane1 = allPlanes.at(planeIndices[0]);
        for (std::size_t i = 1; i < planeIndices.size(); ++i) {
            const Plane<T>& plane2 = allPlanes.at(planeIndices[i]);

            if (plane1.getNormal().crossProduct(plane2.getNormal()).squareLength() < PARALLEL_COMPARISON_TOLERANCE) [[unlikely]] { //planes are parallel: continue on next plane
                continue;
            }

            for (std::size_t j = i + 1; j < planeIndices.size(); ++j) {
                const Plane<T>& plane3 = allPlanes.at(planeIndices[j]);

                Vector3<T> n2CrossN3 = plane2.getNormal().crossProduct(plane3.getNormal());
                if (n2CrossN3.squareLength() < 0.0
                        || plane3.getNormal().crossProduct(plane1.getNormal()).squareLength() < PARALLEL_COMPARISON_TOLERANCE
                        || plane1.getNormal().dotProduct(n2CrossN3) == 0.0) { //additional check due to float imprecision
                    continue; //planes are parallel: continue on next plane
                }

                nonParallelPlanes = {plane1, plane2, plane3};
                return nonParallelPlanes;
            }
        }
        return nonParallelPlanes;
    }

    //explicit template
    template class ResizeConvexHull3DService<float>;

    template class ResizeConvexHull3DService<double>;

}
