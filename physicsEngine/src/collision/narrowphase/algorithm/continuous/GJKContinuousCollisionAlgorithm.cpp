#include <limits>
#include <sstream>
#include "UrchinCommon.h"

#include "collision/narrowphase/algorithm/continuous/GJKContinuousCollisionAlgorithm.h"
#include "collision/narrowphase/algorithm/utils/Simplex.h"

namespace urchin
{

	template<class T, class OUT> GJKContinuousCollisionAlgorithm<T, OUT>::GJKContinuousCollisionAlgorithm() :
		squareEpsilon(std::numeric_limits<T>::epsilon() * std::numeric_limits<T>::epsilon()),
		maxIteration(ConfigService::instance()->getUnsignedIntValue("narrowPhase.gjkContinuousCollisionMaxIteration")),
		terminationTolerance(ConfigService::instance()->getFloatValue("narrowPhase.gjkContinuousCollisionTerminationTolerance"))
	{

	}

	template<class T, class OUT> std::shared_ptr<ContinuousCollisionResult<OUT>> GJKContinuousCollisionAlgorithm<T, OUT>::calculateTimeOfImpact(const TemporalObject &object1, const TemporalObject &object2,
			AbstractWorkBody *body2) const
	{
		T timeToHit = 0.0; //0.0 represents initial situation (from transformation), 1.0 represents final situation (to transformation).
		Vector3<T> normalFromObject2;
		Simplex<T> simplex;

		PhysicsTransform interpolatedTransform1 = object1.getFrom();
		PhysicsTransform interpolatedTransform2 = object2.getFrom();

		Vector3<T> linearVelocity1 = object1.getFrom().getPosition().vector(object1.getTo().getPosition()).template cast<T>();
		Vector3<T> linearVelocity2 = object2.getFrom().getPosition().vector(object2.getTo().getPosition()).template cast<T>();
		Vector3<T> relativeMotion = linearVelocity1 - linearVelocity2;

		Point3<T> initialSupportPoint1 = getWorldSupportPoint(object1, -relativeMotion, interpolatedTransform1);
		Point3<T> initialSupportPoint2 = getWorldSupportPoint(object2, relativeMotion, interpolatedTransform2);
		Point3<T> initialPoint = initialSupportPoint1 - initialSupportPoint2;

		Vector3<T> direction = (-initialPoint).toVector();

		for(unsigned int iterationNumber=0; iterationNumber<maxIteration; ++iterationNumber)
		{
			Point3<T> supportPoint1 = getWorldSupportPoint(object1, direction, interpolatedTransform1);
			Point3<T> supportPoint2 = getWorldSupportPoint(object2, -direction, interpolatedTransform2);
			Point3<T> newPoint = supportPoint1 - supportPoint2;

			const Vector3<T> &vClosestPoint = -direction; //vector from origin to closest point of simplex
			T closestPointDotNewPoint = vClosestPoint.dotProduct(newPoint.toVector());

			if (timeToHit > 1.0)
			{ //no hit detected between from and to transformation
				return std::shared_ptr<ContinuousCollisionResult<OUT>>(nullptr);
			}

			if (closestPointDotNewPoint > 0.0)
			{
				T closestPointDotRelativeMotion = vClosestPoint.dotProduct(relativeMotion);
				if (closestPointDotRelativeMotion >= -squareEpsilon)
				{
					return std::shared_ptr<ContinuousCollisionResult<OUT>>(nullptr);
				}

				timeToHit = timeToHit - closestPointDotNewPoint / closestPointDotRelativeMotion;

				interpolatedTransform1.setPosition(interpolate(object1.getFrom().getPosition(), object1.getTo().getPosition(), timeToHit));
				interpolatedTransform2.setPosition(interpolate(object2.getFrom().getPosition(), object2.getTo().getPosition(), timeToHit));

				normalFromObject2 = -direction;
			}

			if(!simplex.isPointInSimplex(newPoint))
			{
				simplex.addPoint(supportPoint1, supportPoint2);
			}

			direction = (-simplex.getClosestPointToOrigin()).toVector();
			T closestPointSquareDistance = direction.squareLength();

			if(closestPointSquareDistance < terminationTolerance)
			{
				if(normalFromObject2.squareLength() < squareEpsilon)
				{
					return std::shared_ptr<ContinuousCollisionResult<OUT>>(nullptr);
				}

				normalFromObject2 = normalFromObject2.normalize();

				Point3<T> hitPointOnObject1, hitPointOnObject2;
				simplex.computeClosestPoints(hitPointOnObject1, hitPointOnObject2);

				return std::make_shared<ContinuousCollisionResult<OUT>>(body2, normalFromObject2.template cast<OUT>(), hitPointOnObject2.template cast<OUT>(), (OUT)timeToHit);
			}
		}

		logMaximumIterationReach();

		return std::shared_ptr<ContinuousCollisionResult<OUT>>(nullptr);
	}

	template<class T, class OUT> Point3<T> GJKContinuousCollisionAlgorithm<T, OUT>::getWorldSupportPoint(const TemporalObject &object, const Vector3<T> &globalDirection, const PhysicsTransform &worldTransform) const
	{
		Vector3<float> localDirection = worldTransform.retrieveOrientationMatrix().transpose() * globalDirection.template cast<float>();
		Point3<float> localSupportPoint = object.getLocalObject()->getSupportPoint(localDirection, true);

		return worldTransform.transform(localSupportPoint).template cast<T>();
	}

	template<class T, class OUT> Point3<float> GJKContinuousCollisionAlgorithm<T, OUT>::interpolate(const Point3<float> &from, const Point3<float> &to, T interpolatePercentage) const
	{
		T invertedInterpolatePercentage = 1.0 - interpolatePercentage;

		return Point3<float>(
				invertedInterpolatePercentage * from.X + interpolatePercentage * to.X,
				invertedInterpolatePercentage * from.Y + interpolatePercentage * to.Y,
				invertedInterpolatePercentage * from.Z + interpolatePercentage * to.Z);
	}

	template<class T, class OUT> void GJKContinuousCollisionAlgorithm<T, OUT>::logMaximumIterationReach() const
	{
		std::stringstream logStream;
		logStream<<"Maximum of iteration reached on GJK continuous collision algorithm ("<<maxIteration<<")."<<std::endl;
		logStream<<" - Termination tolerance: "<<terminationTolerance;
		Logger::logger().logWarning(logStream.str());
	}

	//explicit template
	template class GJKContinuousCollisionAlgorithm<float, float>;
	template class GJKContinuousCollisionAlgorithm<double, float>;
	template class GJKContinuousCollisionAlgorithm<double, double>;

}
