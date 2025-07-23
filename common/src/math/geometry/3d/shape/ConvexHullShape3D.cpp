#include <algorithm>
#include <limits>
#include <ranges>

#include "math/geometry/3d/shape/ConvexHullShape3D.h"
#include "math/geometry/3d/util/ResizeConvexHull3DService.h"
#include "math/geometry/3d/object/ConvexHull3D.h"
#include "math/algebra/point/Point4.h"
#include "logger/Logger.h"

namespace urchin {
    /**
     * @param points Points used to construct the convex hull shape. Points inside the convex hull shape are accepted but will unused.
     */
    template<class T> ConvexHullShape3D<T>::ConvexHullShape3D(const std::vector<Point3<T>>& points) :
            nextPointIndex(0),
            nextTriangleIndex(0) {
        //build tetrahedron
        std::set<std::size_t> pointsToExclude = buildTetrahedron(points);

        //add each point to the tetrahedron
        for (std::size_t i = 0; i < points.size(); i++) {
            if (!pointsToExclude.contains(i)) [[likely]] {
                addNewPoint(points[i]);
            }
        }
    }

    /**
     * @param points Points of the convex hull shape (all points must belong to the convex hull shape)
     * @param indexedTriangles Triangles of the convex hull shape (the triangles must form a convex)
     */
    template<class T> ConvexHullShape3D<T>::ConvexHullShape3D(const std::map<std::size_t, ConvexHullPoint<T>>& points, const std::map<std::size_t, IndexedTriangle3D<T>>& indexedTriangles) :
            nextPointIndex(points.rbegin()->first + 1),
            nextTriangleIndex(indexedTriangles.rbegin()->first + 1),
            points(points),
            indexedTriangles(indexedTriangles) {

    }

    /**
     * Points of convex hull shape indexed to be used with indexed triangles.
     */
    template<class T> const std::map<std::size_t, ConvexHullPoint<T>>& ConvexHullShape3D<T>::getConvexHullPoints() const {
        return points;
    }

    /**
     * Points of convex hull shape. Order of points is undetermined.
     */
    template<class T> std::vector<Point3<T>> ConvexHullShape3D<T>::getPoints() const {
        std::vector<Point3<T>> result;
        result.reserve(points.size());

        for (const auto& it : points) {
            result.push_back(it.second.point);
        }

        return result;
    }

    /**
     * Triangles of convex hull shape where points are sorted in counterclockwise direction in a right hand coordinate system (Z+ directed to the observer).
     */
    template<class T> const std::map<std::size_t, IndexedTriangle3D<T>>& ConvexHullShape3D<T>::getIndexedTriangles() const {
        return indexedTriangles;
    }

    /**
     * @return Returns index of point added. If point doesn't make part of convex, result is zero.
     */
    template<class T> std::size_t ConvexHullShape3D<T>::addNewPoint(const Point3<T>& newPoint) {
        std::vector<std::size_t> removedTriangleIndices;
        removedTriangleIndices.reserve(4);

        return addNewPoint(newPoint, removedTriangleIndices);
    }

    /**
    * @param removedTriangleIndices [out] Indices of removed triangles from convex hull shape
    * @return Returns index of point added. If point doesn't make part of convex, result is zero.
    */
    template<class T> std::size_t ConvexHullShape3D<T>::addNewPoint(const Point3<T>& newPoint, std::vector<std::size_t>& removedTriangleIndices) {
        std::map<uint_fast64_t, std::pair<std::size_t, std::size_t>> edges;

        //deletes all triangles visible by the new point
        unsigned int trianglesRemoved = 0;
        for (auto itTriangle = indexedTriangles.begin(); itTriangle != indexedTriangles.end();) {
            const IndexedTriangle3D<T> indexedTriangle = itTriangle->second;
            const Vector3<T>& triangleNormal = indexedTriangle.computeNormal(
                    points.at(indexedTriangle.getIndex(0)).point,
                    points.at(indexedTriangle.getIndex(1)).point,
                    points.at(indexedTriangle.getIndex(2)).point);

            const Point3<T>& point0 = points.at(indexedTriangle.getIndex(0)).point;
            Vector3<T> triangleToPoint = point0.vector(newPoint);

            if (triangleNormal.dotProduct(triangleToPoint) > 0.0) {
                for (std::size_t i = 0; i < 3; i++) { //each edge
                    std::size_t index1 = indexedTriangle.getIndices()[i];
                    std::size_t index2 = indexedTriangle.getIndices()[(i + 1) % 3];

                    auto edgeId = (uint_fast64_t)std::min(index1, index2);
                    edgeId = edgeId << 32u;
                    edgeId = edgeId + std::max(index1, index2);

                    auto itEdge = edges.find(edgeId);
                    if (itEdge == edges.end()) {
                        edges[edgeId] = std::make_pair(index1, index2);
                    } else {
                        edges.erase(itEdge);
                    }
                }

                removedTriangleIndices.push_back(itTriangle->first);
                removeTriangle(itTriangle++);
                trianglesRemoved++;
            } else {
                ++itTriangle;
            }
        }

        //adds the new triangles
        if (!edges.empty()) {
            std::size_t newPointIndex = nextPointIndex++;
            points[newPointIndex].point = newPoint;

            for (const auto& edgeIndices : std::views::values(edges)) {
                addTriangle(IndexedTriangle3D<T>(edgeIndices.first, edgeIndices.second, newPointIndex));
            }

            if (points[newPointIndex].triangleIndices.size() < 3) [[unlikely]] {
                logConvexHullData("Add new point on convex hull: new point (index: " + std::to_string(newPointIndex) + ") having less then 3 triangles");
            }

            return newPointIndex;
        }

        if (edges.empty() && trianglesRemoved > 0) [[unlikely]] {
            std::stringstream logNewPointStream;
            logNewPointStream.precision(std::numeric_limits<T>::max_digits10);
            logNewPointStream << newPoint;
            logConvexHullData("Add new point on convex hull: triangles removed but no new point added (value: " + logNewPointStream.str() + ")");
        }

        return 0;
    }

    template<class T> Point3<T> ConvexHullShape3D<T>::getSupportPoint(const Vector3<T>& direction) const {
        T maxPointDotDirection = points.begin()->second.point.toVector().dotProduct(direction);
        Point3<T> maxPoint = points.begin()->second.point;

        for (const auto& itPoints : points) {
            T currentPointDotDirection = itPoints.second.point.toVector().dotProduct(direction);
            if (currentPointDotDirection > maxPointDotDirection) {
                maxPointDotDirection = currentPointDotDirection;
                maxPoint = itPoints.second.point;
            }
        }

        return maxPoint;
    }

    /**
     * @param distance All planes of convex hull shape will be moved along their normal to the specified distance.
     * Positive distance will extend convex hull shape and negative distance will shrink the convex hull shape.
     * @return Convex hull shape resized. If resize is impossible to keep plane normal outside convex hull shape: nullptr is returned.
     */
    template<class T> std::unique_ptr<ConvexHullShape3D<T>> ConvexHullShape3D<T>::resize(T distance) const {
        return ResizeConvexHull3DService<T>::resizeConvexHullShape(*this, distance);
    }

    template<class T> std::unique_ptr<ConvexShape3D<T>> ConvexHullShape3D<T>::clone() const {
        return std::make_unique<ConvexHullShape3D<T>>(*this);
    }

    template<class T> std::unique_ptr<ConvexObject3D<T>> ConvexHullShape3D<T>::toConvexObject(const Transform<T>& transform) const {
        std::map<std::size_t, ConvexHullPoint<T>> transformedConvexHullPoints = points;
        for (auto& it : transformedConvexHullPoints) {
            it.second.point = (transform.getTransformMatrix() * Point4<T>(it.second.point)).toPoint3();
        }

        return std::make_unique<ConvexHull3D<T>>(ConvexHullShape3D<T>(transformedConvexHullPoints, indexedTriangles));
    }

    template<class T> void ConvexHullShape3D<T>::addTriangle(const IndexedTriangle3D<T>& indexedTriangle) {
        //add indexed triangles to triangles map
        std::size_t triangleIndex = nextTriangleIndex++;
        indexedTriangles.emplace(triangleIndex, indexedTriangle);

        //add triangles reference on points
        for (std::size_t i = 0; i < 3; i++) {
            points[indexedTriangle.getIndex(i)].triangleIndices.push_back(triangleIndex);
        }
    }

    template<class T> void ConvexHullShape3D<T>::removeTriangle(const typename std::map<std::size_t, IndexedTriangle3D<T>>::iterator& itTriangle) {
        //remove reference of triangles on points
        const std::array<std::size_t, 3>& indices = itTriangle->second.getIndices();
        for (std::size_t index : indices) {
            std::vector<std::size_t>& pointTriangles = points.at(index).triangleIndices;
            std::erase(pointTriangles, itTriangle->first);
            if (pointTriangles.empty()) { //orphan point: remove it
                points.erase(index);
            }
        }

        //remove indexed triangles from triangle map
        indexedTriangles.erase(itTriangle);
    }

    /**
     * @return All indices points used to build the tetrahedron
     */
    template<class T> std::set<std::size_t> ConvexHullShape3D<T>::buildTetrahedron(const std::vector<Point3<T>>& points) {
        //1. initialize
        std::set<std::size_t> pointsUsed;

        //2. build a point (use first point)
        if (points.empty()) [[unlikely]] {
            throw buildException(points, pointsUsed);
        }

        this->points[nextPointIndex++].point = points[0];
        pointsUsed.insert(0);

        //3. build a line (find two distinct points)
        for (std::size_t i = 1; i < points.size(); i++) {
            if (points[i] != this->points[0].point) [[likely]] {
                this->points[nextPointIndex++].point = points[i];
                pointsUsed.insert(i);
                break;
            }
        }

        if (pointsUsed.size() != 2) [[unlikely]] {
            throw buildException(points, pointsUsed);
        }

        //4. build triangles (find a point which doesn't belong to line).
        Vector3<T> lineVector = this->points[0].point.vector(this->points[1].point);
        for (std::size_t i = 1; i < points.size(); i++) {
            if (pointsUsed.contains(i)) { //point already used to build the tetrahedron
                continue;
            }

            Vector3<T> linesCrossProduct = lineVector.crossProduct(this->points[0].point.vector(points[i]));
            if (linesCrossProduct.X != (T)0.0 || linesCrossProduct.Y != (T)0.0 || linesCrossProduct.Z != (T)0.0) {
                this->points[nextPointIndex++].point = points[i];
                pointsUsed.insert(i);
                break;
            }
        }

        if (pointsUsed.size() != 3) [[unlikely]] {
            throw buildException(points, pointsUsed);
        }

        std::size_t triangleIndex1 = nextTriangleIndex++;
        std::size_t triangleIndex2 = nextTriangleIndex++;
        indexedTriangles.emplace(triangleIndex1, IndexedTriangle3D<T>(0, 1, 2));
        indexedTriangles.emplace(triangleIndex2, IndexedTriangle3D<T>(0, 2, 1));
        for (auto& it : this->points) {
            it.second.triangleIndices.push_back(triangleIndex1);
            it.second.triangleIndices.push_back(triangleIndex2);
        }

        //5. build tetrahedron (find a no coplanar point to the triangle)
        const IndexedTriangle3D<T> firstIndexedTriangle = this->indexedTriangles.at(0);
        const Vector3<T>& firstTriangleNormal = firstIndexedTriangle.computeNormal(
                this->points.at(firstIndexedTriangle.getIndex(0)).point,
                this->points.at(firstIndexedTriangle.getIndex(1)).point,
                this->points.at(firstIndexedTriangle.getIndex(2)).point);
        const Point3<T>& firstPoint = this->points.at(0).point;
        for (std::size_t i = 1; i < points.size(); i++) {
            if (pointsUsed.contains(i)) { //point already used to build the tetrahedron
                continue;
            }

            Vector3<T> triangleToPoint = firstPoint.vector(points[i]);
            if (firstTriangleNormal.dotProduct(triangleToPoint) != (T)0.0) {
                addNewPoint(points[i]);
                pointsUsed.insert(i);
                break;
            }
        }

        if (pointsUsed.size() != 4) [[unlikely]] {
            throw buildException(points, pointsUsed);
        }

        for (unsigned int i = 0; i < 4; ++i) {
            if (this->points[i].triangleIndices.size() < 3) [[unlikely]] {
                logConvexHullData("Initial convex hull tetrahedron built with a point having less then 3 triangles");
                break;
            }
        }

        return pointsUsed;
    }

    /**
     * Build an exception during building of tetrahedron
     * @param pointsUsed Points currently used to construct the tetrahedron
     * @return Exception during building of tetrahedron
     */
    template<class T> std::invalid_argument ConvexHullShape3D<T>::buildException(const std::vector<Point3<T>>& points, const std::set<std::size_t>& pointsUsed) {
        std::string formName;
        if (pointsUsed.empty()) {
            formName = "empty set";
        } else if (pointsUsed.size() == 1) {
            formName = "point";
        } else if (pointsUsed.size() == 2) {
            formName = "line";
        } else if (pointsUsed.size() == 3) {
            formName = "plane";
        } else {
            return std::invalid_argument("Number of points used to build the tetrahedron unsupported: " + std::to_string(pointsUsed.size()) + ".");
        }

        //log points in error log file
        std::stringstream logStream;
        if (!points.empty()) {
            logStream << "Impossible to build a convex hull shape with following points:" << std::endl;
            for (std::size_t i = 0; i < points.size(); ++i) {
                logStream << " - " << points[i] << std::endl;
            }
        } else {
            logStream << "Impossible to build a convex hull shape with zero point.";
        }
        Logger::instance().logError(logStream.str());

        return std::invalid_argument("Impossible to build the convex hull shape. All points form a " + formName + ".");
    }

    template<class T> void ConvexHullShape3D<T>::logConvexHullData(std::string_view errorMessage) const {
        std::stringstream logStream;
        logStream.precision(std::numeric_limits<T>::max_digits10);

        logStream << errorMessage << std::endl;
        logStream << *this;

        Logger::instance().logError(logStream.str());
    }

    template<class T> std::ostream& operator <<(std::ostream& stream, const ConvexHullShape3D<T>& ch) {
        for (auto it = ch.getIndexedTriangles().begin(); it != ch.getIndexedTriangles().end(); ++it) {
            stream << "Triangle " << it->first << ": "
                << "(" << ch.getConvexHullPoints().at(it->second.getIndex(0)).point << ") "
                << "(" << ch.getConvexHullPoints().at(it->second.getIndex(1)).point << ") "
                << "(" << ch.getConvexHullPoints().at(it->second.getIndex(2)).point << ") ";
        }

        return stream;
    }

    //explicit template
    template class ConvexHullShape3D<float>;
    template std::ostream& operator <<<float>(std::ostream&, const ConvexHullShape3D<float>&);
    template struct ConvexHullPoint<float>;

    template class ConvexHullShape3D<double>;
    template std::ostream& operator <<<double>(std::ostream&, const ConvexHullShape3D<double>&);
    template struct ConvexHullPoint<double>;

}
