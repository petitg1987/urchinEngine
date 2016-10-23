#include <limits>
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

	template<class T, class OUT> GJKContinuousCollisionAlgorithm<T, OUT>::~GJKContinuousCollisionAlgorithm()
	{

	}

	template<class T, class OUT> std::shared_ptr<ContinuousCollisionResult<OUT>> GJKContinuousCollisionAlgorithm<T, OUT>::calculateTimeOfImpact(const TemporalObject &objectA, const TemporalObject &objectB,
			const AbstractWorkBody *bodyB) const
	{
		T timeToHit = 0.0; //0.0 represents initial situation (from transformation), 1.0 represents final situation (to transformation).
		Vector3<T> normal;
		Simplex<T> simplex;

		PhysicsTransform interpolatedTransformA = objectA.getFrom();
		PhysicsTransform interpolatedTransformB = objectB.getFrom();

		Vector3<T> linearVelocityA = objectA.getFrom().getPosition().vector(objectA.getTo().getPosition()).template cast<T>();
		Vector3<T> linearVelocityB = objectB.getFrom().getPosition().vector(objectB.getTo().getPosition()).template cast<T>();
		Vector3<T> relativeMotion = linearVelocityA - linearVelocityB;

		Point3<T> initialSupportPointA = getWorldSupportPoint(objectA, -relativeMotion, interpolatedTransformA);
		Point3<T> initialSupportPointB = getWorldSupportPoint(objectB, relativeMotion, interpolatedTransformB);
		Point3<T> initialPoint = initialSupportPointA - initialSupportPointB;

		Vector3<T> direction = (-initialPoint).toVector();

		for(unsigned int iterationNumber=0; iterationNumber<maxIteration; ++iterationNumber)
		{
			Point3<T> supportPointA = getWorldSupportPoint(objectA, direction, interpolatedTransformA);
			Point3<T> supportPointB = getWorldSupportPoint(objectB, -direction, interpolatedTransformB);
			Point3<T> newPoint = supportPointA - supportPointB;

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
				}else
				{
					timeToHit = timeToHit - closestPointDotNewPoint / closestPointDotRelativeMotion;

					interpolatedTransformA.setPosition(interpolate(objectA.getFrom().getPosition(), objectA.getTo().getPosition(), timeToHit));
					interpolatedTransformB.setPosition(interpolate(objectB.getFrom().getPosition(), objectB.getTo().getPosition(), timeToHit));

					normal = -direction;
				}
			}

			if(!simplex.isPointInSimplex(newPoint))
			{
				simplex.addPoint(supportPointA, supportPointB);
			}

			direction = (-simplex.getClosestPointToOrigin()).toVector();
			T closestPointSquareDistance = direction.squareLength();

			if(closestPointSquareDistance < terminationTolerance)
			{
				if(normal.squareLength() < squareEpsilon)
				{
					return std::shared_ptr<ContinuousCollisionResult<OUT>>(nullptr);
				}

				normal = normal.normalize();

				Point3<T> hitPointA, hitPointB;
				simplex.computeClosestPoints(hitPointA, hitPointB);

				return std::make_shared<ContinuousCollisionResult<OUT>>(bodyB, normal.template cast<OUT>(), hitPointB.template cast<OUT>(), (OUT)timeToHit);
			}
		}

		#ifdef _DEBUG
			logMaximumIterationReach();
		#endif

		return std::shared_ptr<ContinuousCollisionResult<OUT>>(nullptr);
	}

	template<class T, class OUT> Point3<T> GJKContinuousCollisionAlgorithm<T, OUT>::getWorldSupportPoint(const TemporalObject &object, const Vector3<T> &globalDirection, const PhysicsTransform &worldTransform) const
	{
		Vector3<float> localDirection = worldTransform.getOrientationMatrix().transpose() * globalDirection.template cast<float>();
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

	#ifdef _DEBUG
		template<class T, class OUT> void GJKContinuousCollisionAlgorithm<T, OUT>::logMaximumIterationReach() const
		{
			Logger::setLogger(new FileLogger());
			Logger::logger()<<Logger::prefix(Logger::LOG_WARNING);
			Logger::logger()<<"Maximum of iteration reached on GJK continuous collision algorithm ("<<maxIteration<<")."<<"\n";
			Logger::logger()<<" - Termination tolerance: "<<terminationTolerance<<"\n";
			Logger::setLogger(nullptr);
		}
	#endif

	//explicit template
	template class GJKContinuousCollisionAlgorithm<float, float>;
	template class GJKContinuousCollisionAlgorithm<double, float>;
	template class GJKContinuousCollisionAlgorithm<double, double>;

}
