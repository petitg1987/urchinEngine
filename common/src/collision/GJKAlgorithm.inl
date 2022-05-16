/**
* @param includeMargin Indicate whether algorithm operates on objects with margin
*/
template<class T> template<class CONVEX_OBJ1, class CONVEX_OBJ2> GJKResult<T> GJKAlgorithm<T>::processGJK(const CONVEX_OBJ1& convexObject1, const CONVEX_OBJ2& convexObject2) const {
    ScopeProfiler sp(Profiler::physics(), "processGJK");

    //get point which belongs to the outline of the shape (Minkowski difference)
    Vector3<T> initialDirection = Vector3<T>(1.0, 0.0, 0.0);
    Point3<T> initialSupportPointA = convexObject1.getSupportPoint(initialDirection.template cast<float>()).template cast<T>();
    Point3<T> initialSupportPointB = convexObject2.getSupportPoint((-initialDirection).template cast<float>()).template cast<T>();
    Point3<T> initialPoint = initialSupportPointA - initialSupportPointB;

    Vector3<T> direction = (-initialPoint).toVector();

    Simplex<T> simplex;
    simplex.addPoint(initialSupportPointA, initialSupportPointB);

    for (unsigned int iterationNumber = 0; iterationNumber < MAX_ITERATION; ++iterationNumber) {
        Point3<T> supportPointA = convexObject1.getSupportPoint(direction.template cast<float>()).template cast<T>();
        Point3<T> supportPointB = convexObject2.getSupportPoint((-direction).template cast<float>()).template cast<T>();
        Point3<T> newPoint = supportPointA - supportPointB;

        Vector3<T> vClosestPoint = -direction; //vector from origin to the closest point of simplex
        T closestPointSquareDistance = vClosestPoint.dotProduct(vClosestPoint);
        T closestPointDotNewPoint = vClosestPoint.dotProduct(newPoint.toVector());

        //check termination conditions: new point is not more extreme that existing ones OR new point already exist in simplex
        if ((closestPointSquareDistance-closestPointDotNewPoint) <= TERMINATION_TOLERANCE || simplex.isPointInSimplex(newPoint)) {
            if (closestPointDotNewPoint <= 0.0) { //collision detected
                return GJKResult<T>::newCollideResult(simplex);
            }

            return GJKResult<T>::newNoCollideResult(std::sqrt(closestPointSquareDistance), simplex);
        }

        simplex.addPoint(supportPointA, supportPointB);

        direction = (-simplex.getClosestPointToOrigin()).toVector();
    }

    logMaximumIterationReach(convexObject1, convexObject2);

    return GJKResult<T>::newInvalidResult();
}

template<class T> template<class CONVEX_OBJ1, class CONVEX_OBJ2> void GJKAlgorithm<T>::logMaximumIterationReach(const CONVEX_OBJ1& convexObject1, const CONVEX_OBJ2& convexObject2) const {
    std::stringstream logStream;
    logStream.precision(std::numeric_limits<float>::max_digits10);

    logStream << "Maximum of iteration reached on GJK algorithm (" << MAX_ITERATION << ")." << std::endl;
    logStream << " - Termination tolerance: " << TERMINATION_TOLERANCE << std::endl;
    logStream << " - Convex object 1: " << std::endl << convexObject1.toString() << std::endl;
    logStream << " - Convex object 2: " << std::endl << convexObject2.toString();
    Logger::instance().logWarning(logStream.str());
}
