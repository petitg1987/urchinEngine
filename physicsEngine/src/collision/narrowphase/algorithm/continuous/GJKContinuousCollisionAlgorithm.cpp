#include <limits>
#include <sstream>
#include <UrchinCommon.h>

#include <collision/narrowphase/algorithm/continuous/GJKContinuousCollisionAlgorithm.h>

namespace urchin {

    template<class T, class U> ContinuousCollisionResult<U> GJKContinuousCollisionAlgorithm<T, U>::calculateTimeOfImpact(const TemporalObject& object1, const TemporalObject& object2,
            std::shared_ptr<AbstractBody> body2) const {
        T timeToHit = 0.0; //0.0 represents initial situation (from transformation), 1.0 represents final situation (to transformation).
        Vector3<T> normalFromObject2;
        bool normalFromObject2Defined = false;
        Simplex<T> simplex;

        PhysicsTransform interpolatedTransform1 = object1.getFrom();
        PhysicsTransform interpolatedTransform2 = object2.getFrom();

        Vector3<T> linearVelocity1 = object1.getFrom().getPosition().vector(object1.getTo().getPosition()).cast<T>();
        Vector3<T> linearVelocity2 = object2.getFrom().getPosition().vector(object2.getTo().getPosition()).cast<T>();
        Vector3<T> relativeMotion = linearVelocity1 - linearVelocity2;

        Point3<T> initialSupportPoint1 = getWorldSupportPoint(object1, -relativeMotion, interpolatedTransform1);
        Point3<T> initialSupportPoint2 = getWorldSupportPoint(object2, relativeMotion, interpolatedTransform2);
        Point3<T> initialPoint = initialSupportPoint1 - initialSupportPoint2;

        Vector3<T> direction = (-initialPoint).toVector();

        for (unsigned int iterationNumber = 0; iterationNumber < MAX_ITERATION; ++iterationNumber) {
            Point3<T> supportPoint1 = getWorldSupportPoint(object1, direction, interpolatedTransform1);
            Point3<T> supportPoint2 = getWorldSupportPoint(object2, -direction, interpolatedTransform2);
            Point3<T> newPoint = supportPoint1 - supportPoint2;

            Vector3<T> vClosestPoint = -direction; //vector from origin to the closest point of simplex
            T closestPointDotNewPoint = vClosestPoint.dotProduct(newPoint.toVector());

            if (timeToHit > 1.0) { //hit not detected between from and to transformation
                return ContinuousCollisionResult<U>::newNoResult();
            }

            if (closestPointDotNewPoint > 0.0) {
                T closestPointDotRelativeMotion = vClosestPoint.dotProduct(relativeMotion);
                if (closestPointDotRelativeMotion >= -SQUARE_EPSILON) {
                    return ContinuousCollisionResult<U>::newNoResult();
                }

                timeToHit = timeToHit - closestPointDotNewPoint / closestPointDotRelativeMotion;

                interpolatedTransform1.setPosition(interpolate(object1.getFrom().getPosition(), object1.getTo().getPosition(), timeToHit));
                interpolatedTransform2.setPosition(interpolate(object2.getFrom().getPosition(), object2.getTo().getPosition(), timeToHit));

                normalFromObject2 = -direction;
                normalFromObject2Defined = true;
            }

            if (!simplex.isPointInSimplex(newPoint)) {
                simplex.addPoint(supportPoint1, supportPoint2);
            }

            direction = (-simplex.getClosestPointToOrigin()).toVector();
            T closestPointSquareDistance = direction.squareLength();

            if (closestPointSquareDistance < TERMINATION_TOLERANCE) {
                if (simplex.getSize() == 4 && !normalFromObject2Defined) {
                    if (timeToHit == (T)0.0 && simplex.getClosestPointToOrigin() == Point3<T>((T)0.0, (T)0.0, (T)0.0)) {
                        Point3<T> hitPointOnObject1;
                        Point3<T> hitPointOnObject2;
                        simplex.computeClosestPoints(hitPointOnObject1, hitPointOnObject2);

                        return ContinuousCollisionResult<U>::newResult(std::move(body2), object2.getShapeIndex(), Vector3<U>(1.0, 0.0, 0.0), hitPointOnObject2.template cast<U>(), 0.0);
                    } else {
                        std::string wrongSituation = "Unexpected situation reach on continuous collision algorithm.";
                        logInputData(object1, object2, wrongSituation, Logger::ERROR_LVL);
                        return ContinuousCollisionResult<U>::newNoResult();
                    }
                } else if (normalFromObject2Defined) {
                    normalFromObject2 = normalFromObject2.normalize();

                    Point3<T> hitPointOnObject1;
                    Point3<T> hitPointOnObject2;
                    simplex.computeClosestPoints(hitPointOnObject1, hitPointOnObject2);

                    return ContinuousCollisionResult<U>::newResult(std::move(body2), object2.getShapeIndex(), normalFromObject2.template cast<U>(), hitPointOnObject2.template cast<U>(), (U) timeToHit);
                }
            }
        }

        std::string maxIterationReachMsg = "Maximum of iteration reached on GJK continuous collision algorithm (" + std::to_string(MAX_ITERATION) + ").";
        logInputData(object1, object2, maxIterationReachMsg, Logger::WARNING_LVL);
        return ContinuousCollisionResult<U>::newNoResult();
    }

    template<class T, class U> Point3<T> GJKContinuousCollisionAlgorithm<T, U>::getWorldSupportPoint(const TemporalObject& object, const Vector3<T>& globalDirection, const PhysicsTransform& worldTransform) const {
        Vector3<float> localDirection = worldTransform.retrieveOrientationMatrix().transpose() * globalDirection.template cast<float>();
        Point3<float> localSupportPoint = object.getLocalObject().getSupportPoint(localDirection, true);

        return worldTransform.transform(localSupportPoint).cast<T>();
    }

    template<class T, class U> Point3<float> GJKContinuousCollisionAlgorithm<T, U>::interpolate(const Point3<float>& from, const Point3<float>& to, T interpolatePercentage) const {
        auto invertedInterpolatePercentage = (float)(1.0 - interpolatePercentage);

        return Point3(
                invertedInterpolatePercentage * from.X + (float)interpolatePercentage * to.X,
                invertedInterpolatePercentage * from.Y + (float)interpolatePercentage * to.Y,
                invertedInterpolatePercentage * from.Z + (float)interpolatePercentage * to.Z);
    }

    template<class T, class U> void GJKContinuousCollisionAlgorithm<T, U>::logInputData(const TemporalObject& object1, const TemporalObject& object2,
            std::string_view message, Logger::CriticalityLevel logLevel) const {
        std::stringstream logStream;
        logStream.precision(std::numeric_limits<float>::max_digits10);

        logStream << message << std::endl;
        logStream << " - Termination tolerance: " << TERMINATION_TOLERANCE << std::endl;
        logStream << " - Object 1: " << std::endl << object1.getLocalObject().toString() << std::endl;
        logStream << " - Object 1, from: " << object1.getFrom() << std::endl;
        logStream << " - Object 1, to: " << object1.getTo() << std::endl;
        logStream << " - Object 2: " << std::endl << object2.getLocalObject().toString() << std::endl;
        logStream << " - Object 2, from: " << object2.getFrom() << std::endl;
        logStream << " - Object 2, to: " << object2.getTo();
        Logger::instance().log(logLevel, logStream.str());
    }

    //explicit template
    template class GJKContinuousCollisionAlgorithm<float, float>;
    template class GJKContinuousCollisionAlgorithm<double, float>;
    template class GJKContinuousCollisionAlgorithm<double, double>;

}
