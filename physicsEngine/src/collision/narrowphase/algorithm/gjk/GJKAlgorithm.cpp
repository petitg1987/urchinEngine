#include <stdexcept>
#include <cmath>
#include <cassert>

#include "collision/narrowphase/algorithm/gjk/GJKAlgorithm.h"

namespace urchin
{

	template<class T> GJKAlgorithm<T>::GJKAlgorithm() :
		maxIteration(ConfigService::instance()->getUnsignedIntValue("narrowPhase.gjkMaxIteration")),
		relativeTerminationTolerance(ConfigService::instance()->getFloatValue("narrowPhase.gjkRelativeTerminationTolerance")),
		minimumTerminationTolerance(ConfigService::instance()->getFloatValue("narrowPhase.gjkMinimumTerminationTolerance")),
		percentageIncreaseOfMinimumTolerance(ConfigService::instance()->getFloatValue("narrowPhase.gjkPercentageIncreaseOfMinimumTolerance"))
	{

	}

	template<class T> GJKAlgorithm<T>::~GJKAlgorithm()
	{

	}

	/**
	* @param includeMargin Indicate whether algorithm operates on objects with margin
	*/
	template<class T> std::unique_ptr<GJKResult<T>> GJKAlgorithm<T>::processGJK(const CollisionConvexObject3D &convexObject1,
			const CollisionConvexObject3D &convexObject2, bool includeMargin) const
	{
		//get point which belongs to the outline of the shape (Minkowski difference)
		Vector3<T> initialDirection = Vector3<T>(1.0, 0.0, 0.0);
		Point3<T> initialSupportPointA = convexObject1.getSupportPoint(initialDirection.template cast<float>(), includeMargin).template cast<T>();
		Point3<T> initialSupportPointB = convexObject2.getSupportPoint((-initialDirection).template cast<float>(), includeMargin).template cast<T>();
		Point3<T> initialPoint = initialSupportPointA - initialSupportPointB;

		Vector3<T> direction = (-initialPoint).toVector();

		Simplex<T> simplex;
		simplex.addPoint(initialSupportPointA, initialSupportPointB);

		T minimumToleranceMultiplicator = (T)1.0;

		for(unsigned int iterationNumber=0; iterationNumber<maxIteration; ++iterationNumber)
		{
			Point3<T> supportPointA = convexObject1.getSupportPoint(direction.template cast<float>(), includeMargin).template cast<T>();
			Point3<T> supportPointB = convexObject2.getSupportPoint((-direction).template cast<float>(), includeMargin).template cast<T>();
			Point3<T> newPoint = supportPointA - supportPointB;

			const Vector3<T> &vClosestPoint = -direction; //vector from origin to closest point of simplex
			T closestPointSquareDistance = vClosestPoint.dotProduct(vClosestPoint);
			T closestPointDotNewPoint = vClosestPoint.dotProduct(newPoint.toVector());

			//check termination conditions: new point is not more extreme that existing ones OR new point already exist in simplex
			T distanceTolerance = std::max(minimumTerminationTolerance*minimumToleranceMultiplicator, relativeTerminationTolerance*closestPointSquareDistance);
			if((closestPointSquareDistance-closestPointDotNewPoint) <= distanceTolerance || simplex.isPointInSimplex(newPoint))
			{
				if(closestPointDotNewPoint <= 0.0)
				{ //collision detected
					return std::make_unique<GJKResultCollide<T>>(simplex);
				}else
				{
					return std::make_unique<GJKResultNoCollide<T>>(std::sqrt(closestPointSquareDistance), simplex);
				}
			}

			simplex.addPoint(supportPointA, supportPointB);

			direction = (-simplex.getClosestPointToOrigin()).toVector();

			minimumToleranceMultiplicator += percentageIncreaseOfMinimumTolerance;
		}

		#ifdef _DEBUG
			logMaximumIterationReach();
		#endif

		return std::make_unique<GJKResultInvalid<T>>();
	}

	#ifdef _DEBUG
		template<class T> void GJKAlgorithm<T>::logMaximumIterationReach() const
		{
			Logger::setLogger(new FileLogger());
			Logger::logger()<<Logger::prefix(Logger::LOG_WARNING);
			Logger::logger()<<"Maximum of iteration reached on GJK algorithm ("<<maxIteration<<")."<<"\n";
			Logger::logger()<<" - Relative termination tolerance: "<<relativeTerminationTolerance<<"\n";
			Logger::logger()<<" - Minimum termination tolerance: "<<minimumTerminationTolerance<<"\n";
			Logger::logger()<<" - Percentage increase of minimum tolerance: "<<percentageIncreaseOfMinimumTolerance<<"\n\n";
			Logger::setLogger(nullptr);
		}
	#endif

	//explicit template
	template class GJKAlgorithm<float>;
	template class GJKAlgorithm<double>;

}
