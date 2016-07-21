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
		terminationTolerance(ConfigService::instance()->getUnsignedIntValue("narrowPhase.gjkRayCastTerminationTolerance"))
	{

	}

	template<class T> GJKRayCastAlgorithm<T>::~GJKRayCastAlgorithm()
	{

	}

	template<class T> std::unique_ptr<RayCastResult<T>> GJKRayCastAlgorithm<T>::calculateTimeOfImpact(const RayCastObject &objectA, const RayCastObject &objectB) const
	{
		T lengthToHit = 0.0; //also called lambda in math documents
		Vector3<T> normal;
		Simplex<T> simplex;

		Vector3<T> linearVelocityA = objectA.getFrom().getPosition().vector(objectA.getTo().getPosition()).template cast<T>();
		Vector3<T> linearVelocityB = objectB.getFrom().getPosition().vector(objectB.getTo().getPosition()).template cast<T>();
		Vector3<T> relativeMotion = linearVelocityA - linearVelocityB;

		Point3<T> initialSupportPointA = objectA.getObject()->getSupportPoint((-relativeMotion).template cast<float>(), true).template cast<T>();
		Point3<T> initialSupportPointB = objectB.getObject()->getSupportPoint(relativeMotion.template cast<float>(), true).template cast<T>();
		Point3<T> initialPoint = initialSupportPointA - initialSupportPointB;

		Vector3<T> direction = (-initialPoint).toVector();

		for(unsigned int iterationNumber=0; iterationNumber<maxIteration; ++iterationNumber)
		{
			Point3<T> supportPointA = objectA.getObject()->getSupportPoint(direction.template cast<float>(), true).template cast<T>();
			Point3<T> supportPointB = objectB.getObject()->getSupportPoint((-direction).template cast<float>(), true).template cast<T>();
			Point3<T> newPoint = supportPointA - supportPointB;

			const Vector3<T> &vClosestPoint = -direction; //vector from origin to closest point of simplex
			T closestPointDotNewPoint = vClosestPoint.dotProduct(newPoint.toVector());

			if (lengthToHit > 1.0) //TODO why 1.0 ? review variable name ?
			{
				return std::make_unique<RayCastResultNoTOI<T>>();
			}

			if (closestPointDotNewPoint > 0.0)
			{
				T closestPointDotRelativeMotion = vClosestPoint.dotProduct(relativeMotion);
				if (closestPointDotRelativeMotion >= -squareEpsilon)
				{
					return std::make_unique<RayCastResultNoTOI<T>>();
				}else
				{
					lengthToHit = lengthToHit - closestPointDotNewPoint / closestPointDotRelativeMotion;

					//TODO do it
					//interpolatedTransA.getOrigin().setInterpolate3(fromA.getOrigin(),toA.getOrigin(),lambda);
					//interpolatedTransB.getOrigin().setInterpolate3(fromB.getOrigin(),toB.getOrigin(),lambda);

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
					return std::make_unique<RayCastResultNoTOI<T>>();
				}

				normal = normal.normalize();
				Point3<T> hitPointA, hitPointB;
				simplex.computeClosestPoints(hitPointA, hitPointB);

				return std::make_unique<RayCastResultTOI<T>>(normal, hitPointA, hitPointB, lengthToHit);
			}
		}

		#ifdef _DEBUG
			logMaximumIterationReach();
		#endif

		return std::make_unique<RayCastResultNoTOI<T>>();
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
