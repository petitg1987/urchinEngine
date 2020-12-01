#include "object/CollisionTriangleObject.h"
#include "collision/narrowphase/algorithm/utils/AlgorithmResultAllocator.h"
#include "collision/narrowphase/algorithm/epa/EPAAlgorithm.h"
#include "utils/property/EagerPropertyLoader.h"

namespace urchin {

    template<class T> EPAAlgorithm<T>::EPAAlgorithm() :
        maxIteration(EagerPropertyLoader::instance()->getNarrowPhaseEpaMaxIteration()),
        terminationTolerance(EagerPropertyLoader::instance()->getNarrowPhaseEpaTerminationTolerance()) {

    }

    template<class T> std::unique_ptr<EPAResult<T>, AlgorithmResultDeleter> EPAAlgorithm<T>::processEPA(const CollisionConvexObject3D& convexObject1, const CollisionConvexObject3D& convexObject2,
            const GJKResult<T>& gjkResult) const {
        assert(gjkResult.isCollide());

        //handle sub triangle cases
        std::unique_ptr<EPAResult<T>, AlgorithmResultDeleter> subTriangleResult = handleSubTriangle(convexObject1, convexObject2);
        if (subTriangleResult) {
            return subTriangleResult;
        }

        //1. check collision exist
        const Simplex<T>& simplex = gjkResult.getSimplex();
        if (simplex.getSize()==1) { //simplex point is the origin
            return AlgorithmResultAllocator::instance()->newEPAResultNoCollide<T>();
        }

        //2. initialize global variables
        std::map<std::size_t, Point3<T>> supportPointsA, supportPointsB;  //first: index of point in convex hull, second: support points
        std::map<std::size_t, EPATriangleData<T>> trianglesData; //first: index of triangle in convex hull, second: EPA triangle data

        //3. create initial convex hull
        std::map<std::size_t, ConvexHullPoint<T>> convexHullPoints;
        std::map<std::size_t, IndexedTriangle3D<T>> indexedTriangles;

        determineInitialPoints(simplex, convexObject1, convexObject2, convexHullPoints, supportPointsA, supportPointsB);
        if (convexHullPoints.size() >= 4) {
            determineInitialTriangles(convexHullPoints, indexedTriangles);
        }

        if (indexedTriangles.size()!=4) {//due to numerical imprecision, it's impossible to create indexed triangles correctly
            return AlgorithmResultAllocator::instance()->newEPAResultInvalid<T>();
        }

        ConvexHullShape3D<T> convexHullShape(convexHullPoints, indexedTriangles);
        for (auto it = indexedTriangles.begin(); it!=indexedTriangles.end(); ++it) {
            trianglesData.insert(std::pair<std::size_t, EPATriangleData<T>>(it->first, createTriangleData(convexHullShape, it->first)));
        }

        //4. find closest plane of extended polytope
        T upperBoundPenDepth = std::numeric_limits<T>::max();
        typename std::map<std::size_t, EPATriangleData<T>>::const_iterator itClosestTriangleData;
        unsigned int iterationNumber = 0;
        Vector3<T> normal;
        T distanceToOrigin;

        while (true) {
            itClosestTriangleData = getClosestTriangleData(trianglesData);
            const EPATriangleData<T>& closestTriangleData = itClosestTriangleData->second;

            normal = closestTriangleData.getNormal();
            distanceToOrigin = closestTriangleData.getDistanceToOrigin();

            if (iterationNumber > maxIteration) { //can happen on spherical forms where EPA algorithm doesn't progress enough fast
                break;
            }

            //get support point in direction of normal
            const Point3<T> supportPointNormal = convexObject1.getSupportPoint(normal.template cast<float>(), true).template cast<T>();
            const Point3<T> supportPointMinusNormal = convexObject2.getSupportPoint((-normal).template cast<float>(), true).template cast<T>();
            const Point3<T> minkowskiDiffPoint = supportPointNormal - supportPointMinusNormal;

            upperBoundPenDepth = std::min(upperBoundPenDepth, std::abs(minkowskiDiffPoint.toVector().dotProduct(normal)));
            bool closeEnough = upperBoundPenDepth <= (1.0+terminationTolerance) * distanceToOrigin;

            if (!closeEnough) { //polytope can be extended in direction of normal: add a new point
                std::vector<std::size_t> removedTriangleIndices;
                std::size_t index = convexHullShape.addNewPoint(minkowskiDiffPoint, removedTriangleIndices);
                if (index==0) { //finally, polytope cannot by extended in direction of normal. Cause: numerical imprecision.
                    break;
                }

                //compute new triangles data
                const std::vector<std::size_t>& newTriangleIndices = convexHullShape.getConvexHullPoints().at(index).triangleIndices;
                for (std::size_t newTriangleIndex : newTriangleIndices) {
                    trianglesData.insert(std::pair<std::size_t, EPATriangleData<T>>(newTriangleIndex, createTriangleData(convexHullShape, newTriangleIndex)));
                }

                //remove triangle data not used anymore
                for (std::size_t removedTriangleIndex : removedTriangleIndices) {
                    trianglesData.erase(removedTriangleIndex);
                }

                //save support points
                supportPointsA[index] = supportPointNormal;
                supportPointsB[index] = supportPointMinusNormal;
            } else { //polytope cannot by extended in direction of normal: solution is found
                break;
            }
            iterationNumber++;
        }

        //5. compute EPA result: normal, penetration depth and contact points of collision
        const EPATriangleData<T>& closestTriangleData = itClosestTriangleData->second;
        const IndexedTriangle3D<T>& indexedTriangle = convexHullShape.getIndexedTriangles().at(itClosestTriangleData->first);
        const std::size_t pointIndex1 = indexedTriangle.getIndex(0);
        const std::size_t pointIndex2 = indexedTriangle.getIndex(1);
        const std::size_t pointIndex3 = indexedTriangle.getIndex(2);

        const Point3<T> contactPointA = closestTriangleData.getBarycentric(0) * supportPointsA[pointIndex1] + closestTriangleData.getBarycentric(1) * supportPointsA[pointIndex2]
                + closestTriangleData.getBarycentric(2) * supportPointsA[pointIndex3];
        const Point3<T> contactPointB = closestTriangleData.getBarycentric(0) * supportPointsB[pointIndex1] + closestTriangleData.getBarycentric(1) * supportPointsB[pointIndex2]
                + closestTriangleData.getBarycentric(2) * supportPointsB[pointIndex3];

        if (Check::instance()->additionalChecksEnable()) {
            const T distanceDelta = contactPointA.vector(contactPointB).length() - distanceToOrigin;
            if (!MathAlgorithm::isZero(distanceDelta, 0.1f)) {
                logInputData("Incoherent EPA distances", convexObject1, convexObject2, gjkResult);
            }
        }

        return AlgorithmResultAllocator::instance()->newEPAResultCollide<T>(contactPointA, contactPointB, normal, distanceToOrigin);
    }

    template<class T> std::unique_ptr<EPAResult<T>, AlgorithmResultDeleter> EPAAlgorithm<T>::handleSubTriangle(const CollisionConvexObject3D& convexObject1,
                                                                                       const CollisionConvexObject3D& convexObject2) const {
        //Handle specific case for triangles mesh. EPA algorithm is not aware of others triangles of the mesh and can lead to wrong results.
        //Example: an object is inside a triangle: EPA could found a contact point which lead to slide the object on X/Z axis. This is wrong since
        //another triangles exist on X/Z. See: EPAConvexObjectTest::overlapCapsuleAndTriangle() & EPAConvexObjectTest::overlapTriangleAndCapsule().
        //Solution: only allow to slide object based on triangle normal.

        bool needSwap;
        const CollisionTriangleObject *triangleObject;
        const CollisionConvexObject3D *otherObject;

        if (convexObject1.getObjectType()==CollisionConvexObject3D::TRIANGLE_OBJECT) {
            triangleObject = dynamic_cast<const CollisionTriangleObject*>(&convexObject1);
            otherObject = &convexObject2;
            needSwap = false;
        } else if (convexObject2.getObjectType()==CollisionConvexObject3D::TRIANGLE_OBJECT) {
            triangleObject = dynamic_cast<const CollisionTriangleObject*>(&convexObject2);
            otherObject = &convexObject1;
            needSwap = true;
        } else {
            return std::unique_ptr<EPAResult<T>, AlgorithmResultDeleter>(nullptr);
        }

        Triangle3D<float> triangle = triangleObject->retrieveTriangle();
        Point3<float> contactPointOther = otherObject->getSupportPoint(-triangle.computeNormal(), true);
        float triangleBarycentrics[3];
        Point3<float> contactPointTriangle = triangle.closestPoint(contactPointOther, triangleBarycentrics);
        if (triangle.projectedPointInsideTriangle(contactPointOther)) {
            float distanceToOrigin = contactPointTriangle.distance(contactPointOther);
            Vector3<T> normal = contactPointOther.vector(contactPointTriangle).normalize().template cast<T>();

            if (needSwap) {
                return AlgorithmResultAllocator::instance()->newEPAResultCollide<T>(contactPointOther.template cast<T>(), contactPointTriangle.template cast<T>(), -normal, distanceToOrigin);
            }

            return AlgorithmResultAllocator::instance()->newEPAResultCollide<T>(contactPointTriangle.template cast<T>(), contactPointOther.template cast<T>(), normal, distanceToOrigin);
        }

        return std::unique_ptr<EPAResult<T>, AlgorithmResultDeleter>(nullptr);
    }

    /**
     * Determine initial points useful for EPA algorithm: points of initial convex hull as well as the linked support points
     * @param simplex Simplex resulting from GJK algorithm
     * @param convexHullPoints [out] Initial points for EPA algorithm
     * @param supportPointsA [out] Support points of object A used to compute indexed points
     * @param supportPointsB [out] Support points of object B used to compute indexed points
     */
    template<class T> void EPAAlgorithm<T>::determineInitialPoints(const Simplex<T>& simplex, const CollisionConvexObject3D& convexObject1,
            const CollisionConvexObject3D& convexObject2, std::map<std::size_t, ConvexHullPoint<T>>& convexHullPoints,
            std::map<std::size_t, Point3<T>>& supportPointsA, std::map<std::size_t, Point3<T>>& supportPointsB) const {
        if (simplex.getSize()==2) { //simplex is a segment line containing the origin
            //compute normalized direction vector
            const Vector3<T> lineDirection = simplex.getPoint(0).vector(simplex.getPoint(1)).normalize();

            //find best coordinate axis for which i-th component of 'lineDirection' has the smallest absolute value.
            //In this way, the angle between 'lineDirection' and best coordinate axis is the largest. Thus the cross
            // product has the largest length.
            int minAxis = 0; //0=X, 1=Y, 2=Z
            T minValue = std::abs(lineDirection.X);
            for (int i=1; i<3; ++i) {
                if (minValue > std::abs(lineDirection[i])) {
                    minValue = std::abs(lineDirection[i]);
                    minAxis = i;
                }
            }

            //create three vectors around 'lineDirection' separate of 120 degrees
            const T sin60 = 0.86602540378; //sin(pi/3)
            const T cos60 = 0.5; //cos(pi/3)
            const Quaternion<T> rotationQuaternion(lineDirection.X*sin60, lineDirection.Y*sin60, lineDirection.Z*sin60, cos60);
            const Matrix3<T> rotationMatrix = rotationQuaternion.toMatrix3();

            const Vector3<T> v1 = lineDirection.crossProduct(Vector3<T>(minAxis==0 ? 1.0 : 0.0, minAxis==1 ? 1.0 : 0.0, minAxis==2 ? 1.0 : 0.0));
            const Vector3<T> v2 = rotationMatrix * Vector3<T>(v1.X, v1.Y, v1.Z);
            const Vector3<T> v3 = rotationMatrix * Vector3<T>(v2.X, v2.Y, v2.Z);

            //retrieve all points forming a hexahedron (two tetrahedron glued together)
            Point3<T> supportPoints[3] = {
                    convexObject1.getSupportPoint(v1.template cast<float>(), true).template cast<T>(),
                    convexObject1.getSupportPoint(v2.template cast<float>(), true).template cast<T>(),
                    convexObject1.getSupportPoint(v3.template cast<float>(), true).template cast<T>()};
            Point3<T> supportPointsMinus[3] = {
                    convexObject2.getSupportPoint((-v1).template cast<float>(), true).template cast<T>(),
                    convexObject2.getSupportPoint((-v2).template cast<float>(), true).template cast<T>(),
                    convexObject2.getSupportPoint((-v3).template cast<float>(), true).template cast<T>()};

            for (std::size_t i=0; i<2; ++i) {
                convexHullPoints[i].point = simplex.getPoint(i);
                supportPointsA[i] = simplex.getSupportPointA(i);
                supportPointsB[i] = simplex.getSupportPointB(i);
            }
            for (std::size_t i=0; i<3; ++i) {
                convexHullPoints[i+2].point = supportPoints[i] - supportPointsMinus[i];
                supportPointsA[i+2] = supportPoints[i];
                supportPointsB[i+2] = supportPointsMinus[i];
            }

            //keep only the tetrahedron containing the origin
            if (Tetrahedron<T>(convexHullPoints[0].point, convexHullPoints[2].point, convexHullPoints[3].point, convexHullPoints[4].point).collideWithPoint(Point3<T>(0.0, 0.0, 0.0))) {
                //we use the point 4 instead of point 1 for the initial tetrahedron
                convexHullPoints[1].point = convexHullPoints[4].point;
                supportPointsA[1] = supportPointsA[4];
                supportPointsB[1] = supportPointsB[4];
            } else if (Tetrahedron<T>(convexHullPoints[4].point, convexHullPoints[1].point, convexHullPoints[2].point, convexHullPoints[3].point).collideWithPoint(Point3<T>(0.0, 0.0, 0.0))) {
                //we use the point 4 instead of point 0 for the initial tetrahedron
                convexHullPoints[0].point = convexHullPoints[4].point;
                supportPointsA[0] = supportPointsA[4];
                supportPointsB[0] = supportPointsB[4];
            } else { //no tetrahedron containing the origin due to float imprecision
                convexHullPoints.clear();
            }
        } else if (simplex.getSize()==3) { //simplex is a triangle containing the origin
            //create two vectors based on three points
            Triangle3D<T> triangle(simplex.getPoint(0), simplex.getPoint(1), simplex.getPoint(2));
            const Vector3<T> v1 = triangle.computeNormal();
            const Vector3<T> v2 = -v1;

            //retrieve all points forming a hexahedron (two tetrahedron glued together)
            Point3<T> supportPoints[2] = {
                    convexObject1.getSupportPoint(v1.template cast<float>(), true).template cast<T>(),
                    convexObject1.getSupportPoint(v2.template cast<float>(), true).template cast<T>()};
            Point3<T> supportPointsMinus[2] = {
                    convexObject2.getSupportPoint((-v1).template cast<float>(), true).template cast<T>(),
                    convexObject2.getSupportPoint((-v2).template cast<float>(), true).template cast<T>()};

            for (std::size_t i=0; i<3; ++i) {
                convexHullPoints[i].point = simplex.getPoint(i);
                supportPointsA[i] = simplex.getSupportPointA(i);
                supportPointsB[i] = simplex.getSupportPointB(i);
            }
            for (std::size_t i=0; i<2; ++i) {
                convexHullPoints[i+3].point = supportPoints[i] - supportPointsMinus[i];
                supportPointsA[i+3] = supportPoints[i];
                supportPointsB[i+3] = supportPointsMinus[i];
            }

            //keep only the tetrahedron containing the origin
            if (Tetrahedron<T>(convexHullPoints[0].point, convexHullPoints[1].point, convexHullPoints[2].point, convexHullPoints[3].point).collideWithPoint(Point3<T>(0.0, 0.0, 0.0))) {
                //we use the 4 first point - nothing to do
            } else if (Tetrahedron<T>(convexHullPoints[0].point, convexHullPoints[1].point, convexHullPoints[2].point, convexHullPoints[4].point).collideWithPoint(Point3<T>(0.0, 0.0, 0.0))) {
                //we use the point 4 instead of point 3 for the initial tetrahedron
                convexHullPoints[3].point = convexHullPoints[4].point;
                supportPointsA[3] = supportPointsA[4];
                supportPointsB[3] = supportPointsB[4];
            } else { //no tetrahedron containing the origin due to float imprecision
                convexHullPoints.clear();
            }
        } else if (simplex.getSize()==4) { //simplex is a tetrahedron containing the origin
            for (std::size_t i=0; i<4; ++i) {
                convexHullPoints[i].point = simplex.getPoint(i);
                supportPointsA[i] = simplex.getSupportPointA(i);
                supportPointsB[i] = simplex.getSupportPointB(i);
            }
        } else {
            throw std::invalid_argument("Size of simplex unsupported: " + std::to_string(simplex.getSize()) + ".");
        }
    }

    /**
     * Determine triangles of initial convex hull of EPA algorithm. Normal of triangle must be outside the convex hull.
     * @param convexHullPoints [in/out] Points of initial convex hull. Linked triangles to points are updated.
     * @param indexedTriangles [out] Triangle of initial convex hull. When points are too close together or almost on same
     * plane: the indexed triangles can be incomplete
     */
    template<class T> void EPAAlgorithm<T>::determineInitialTriangles(std::map<std::size_t, ConvexHullPoint<T>>& convexHullPoints,
            std::map<std::size_t, IndexedTriangle3D<T>>& indexedTriangles) const {
        for (std::size_t i=0; i<3; ++i) {
            for (std::size_t j=i+1; j<4; ++j) {
                T distance = convexHullPoints.at(i).point.vector(convexHullPoints.at(j).point).length();
                T minPointsDistance = (std::nextafter(distance, std::numeric_limits<T>::max()) - distance) * 10.0;

                if (distance < minPointsDistance) {
                    return;
                }
            }
        }

        constexpr std::size_t indices[4][3] =         {{0, 1, 2}, {0, 3, 1}, {0, 2, 3}, {1, 3, 2}};
        constexpr std::size_t revIndices[4][3] =     {{0, 2, 1}, {0, 1, 3}, {0, 3, 2}, {1, 2, 3}};
        for (std::size_t i=0; i<4; ++i) {
            const std::size_t pointOutsideTriangle = 6 - (indices[i][0] + indices[i][1] + indices[i][2]);
            const Vector3<T> normalTriangle = IndexedTriangle3D<T>(indices[i]).computeNormal(
                    convexHullPoints.at(indices[i][0]).point,
                    convexHullPoints.at(indices[i][1]).point,
                    convexHullPoints.at(indices[i][2]).point);
            const Vector3<T> trianglePointToOutsidePoint = convexHullPoints.at(indices[i][0]).point.vector(convexHullPoints.at(pointOutsideTriangle).point);
            T dotProduct = normalTriangle.dotProduct(trianglePointToOutsidePoint);

            T trianglePointToOutsidePointLength = trianglePointToOutsidePoint.length();
            T dotProductTolerance = (std::nextafter(trianglePointToOutsidePointLength, std::numeric_limits<T>::max()) - trianglePointToOutsidePointLength);

            if (dotProduct < -dotProductTolerance) {
                indexedTriangles.insert(std::pair<std::size_t, IndexedTriangle3D<T>>(i, IndexedTriangle3D<T>(indices[i])));
                for (std::size_t pointI=0; pointI<3; ++pointI) {
                    convexHullPoints.at(indices[i][pointI]).triangleIndices.push_back(i);
                }
            } else if (dotProduct > dotProductTolerance) {
                indexedTriangles.insert(std::pair<std::size_t, IndexedTriangle3D<T>>(i, IndexedTriangle3D<T>(revIndices[i])));
                for (std::size_t pointI=0; pointI<3; ++pointI) {
                    convexHullPoints.at(revIndices[i][pointI]).triangleIndices.push_back(i);
                }
            } else {
                return;
            }
        }
    }

    /**
     * Find closest plane (triangle plane) near the origin
     * @param trianglesData Triangles data
     * @return Iterator on closest triangle data
     */
    template<class T> typename std::map<std::size_t, EPATriangleData<T>>::const_iterator EPAAlgorithm<T>::getClosestTriangleData(
            const typename std::map<std::size_t, EPATriangleData<T>>& trianglesData) const {
        T minDistanceToOrigin = std::numeric_limits<T>::max();
        typename std::map<std::size_t, EPATriangleData<T>>::const_iterator closestTriangleData;

        for (auto it = trianglesData.begin(); it!=trianglesData.end(); ++it) {
            const EPATriangleData<T>& triangleData = it->second;
            T distanceToOrigin = std::abs(triangleData.getDistanceToOrigin());

            if (minDistanceToOrigin > distanceToOrigin) {
                minDistanceToOrigin = distanceToOrigin;
                closestTriangleData = it;
            }
        }

        return closestTriangleData;
    }

    /**
     * @param triangleIndex Index of triangle in convex hull shape used to compute the data
     * @return Computed triangle data (normal, distance to origin...).
     */
    template<class T> EPATriangleData<T> EPAAlgorithm<T>::createTriangleData(const ConvexHullShape3D<T>& convexHullShape, std::size_t triangleIndex) const {
        const IndexedTriangle3D<T>& indexedTriangle = convexHullShape.getIndexedTriangles().at(triangleIndex);
        const std::size_t pointIndex1 = indexedTriangle.getIndex(0);
        const std::size_t pointIndex2 = indexedTriangle.getIndex(1);
        const std::size_t pointIndex3 = indexedTriangle.getIndex(2);

        const Point3<T>& point1 = convexHullShape.getConvexHullPoints().at(pointIndex1).point;
        const Point3<T>& point2 = convexHullShape.getConvexHullPoints().at(pointIndex2).point;
        const Point3<T>& point3 = convexHullShape.getConvexHullPoints().at(pointIndex3).point;
        const Triangle3D<T> triangle(point1, point2, point3);

        //compute point on the triangle nearest to origin
        T barycentrics[3];
        Point3<T> closestPointToOrigin = triangle.closestPoint(Point3<T>(0.0, 0.0, 0.0), barycentrics);

        //compute minimum distance between triangle and the origin
        T distanceToOrigin = closestPointToOrigin.toVector().length();

        //compute normal (external to convex hull)
        const Vector3<T> normal = triangle.computeNormal();

        return EPATriangleData<T>(distanceToOrigin, normal, closestPointToOrigin, barycentrics);
    }

    template<class T> void EPAAlgorithm<T>::logInputData(const std::string& errorMessage, const CollisionConvexObject3D& convexObject1,
            const CollisionConvexObject3D& convexObject2, const GJKResult<T>& gjkResult) const {
        std::stringstream logStream;
        logStream.precision(std::numeric_limits<T>::max_digits10);

        logStream << errorMessage << std::endl;
        logStream << "Convex object 1: " << std::endl << convexObject1.toString() << std::endl;
        logStream << "Convex object 2: " << std::endl<< convexObject2.toString() << std::endl;
        logStream << "Simplex: " << std::endl << gjkResult.getSimplex();

        Logger::instance()->logError(logStream.str());
    }

    //explicit template
    template class EPAAlgorithm<float>;
    template class EPAAlgorithm<double>;

}
