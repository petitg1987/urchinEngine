#include <limits>
#include "UrchinCommon.h"

#include "collision/narrowphase/algorithm/raycast/GJKRayCastAlgorithm.h"
#include "collision/narrowphase/algorithm/raycast/RayCastResultTOI.h"
#include "collision/narrowphase/algorithm/raycast/RayCastResultNoTOI.h"
#include "collision/narrowphase/algorithm/utils/Simplex.h"

namespace urchin
{

	template<class T> GJKRayCastAlgorithm<T>::GJKRayCastAlgorithm() :
		squareEpsilon(std::numeric_limits<T>::epsilon() * std::numeric_limits<T>::epsilon()),
		maxIteration(ConfigService::instance()->getUnsignedIntValue("narrowPhase.gjkRayCastMaxIteration")),
		terminationTolerance(ConfigService::instance()->getFloatValue("narrowPhase.gjkRayCastTerminationTolerance"))
	{

	}

	template<class T> GJKRayCastAlgorithm<T>::~GJKRayCastAlgorithm()
	{

	}

	template<class T> std::shared_ptr<RayCastResult<T>> GJKRayCastAlgorithm<T>::calculateTimeOfImpact(const RayCastObject &objectA, const RayCastObject &objectB) const
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
				return std::make_shared<RayCastResultNoTOI<T>>();
			}

			if (closestPointDotNewPoint > 0.0)
			{
				T closestPointDotRelativeMotion = vClosestPoint.dotProduct(relativeMotion);
				if (closestPointDotRelativeMotion >= -squareEpsilon)
				{
					return std::make_shared<RayCastResultNoTOI<T>>();
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
					return std::make_shared<RayCastResultNoTOI<T>>();
				}

				normal = normal.normalize(); //TODO see btCollisionWorld:300 ?

				Point3<T> hitPointA, hitPointB;
				simplex.computeClosestPoints(hitPointA, hitPointB);

				return std::make_shared<RayCastResultTOI<T>>(normal, hitPointB, timeToHit);
			}
		}

		#ifdef _DEBUG
			logMaximumIterationReach();
		#endif

		return std::make_unique<RayCastResultNoTOI<T>>();
	}

	template<class T> Point3<T> GJKRayCastAlgorithm<T>::getWorldSupportPoint(const RayCastObject &object, const Vector3<T> &globalDirection, const PhysicsTransform &worldTransform) const
	{
		Vector3<float> localDirection = worldTransform.getOrientation().rotatePoint(Point3<T>(globalDirection).template cast<float>()).toVector();
		Point3<float> localSupportPoint = object.getLocalObject()->getSupportPoint(localDirection, true);

		return worldTransform.transform(localSupportPoint).template cast<T>();
	}

	template<class T> Point3<float> GJKRayCastAlgorithm<T>::interpolate(const Point3<float> &from, const Point3<float> &to, T interpolatePercentage) const
	{
		T invertedInterpolatePercentage = 1.0 - interpolatePercentage;

		return Point3<float>(
				invertedInterpolatePercentage * from.X + interpolatePercentage * to.X,
				invertedInterpolatePercentage * from.Y + interpolatePercentage * to.Y,
				invertedInterpolatePercentage * from.Z + interpolatePercentage * to.Z);
	}

	#ifdef _DEBUG
		template<class T> void GJKRayCastAlgorithm<T>::logMaximumIterationReach() const
		{
			Logger::setLogger(new FileLogger());
			Logger::logger()<<Logger::prefix(Logger::LOG_WARNING);
			Logger::logger()<<"Maximum of iteration reached on Ray Cast algorithm ("<<maxIteration<<")."<<"\n";
			Logger::logger()<<" - Termination tolerance: "<<terminationTolerance<<"\n";
			Logger::setLogger(nullptr);
		}
	#endif

	//explicit template
	template class GJKRayCastAlgorithm<float>;
	template class GJKRayCastAlgorithm<double>;

}
