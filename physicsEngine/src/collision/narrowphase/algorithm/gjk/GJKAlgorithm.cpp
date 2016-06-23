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
	{ //GJK algorithm (see http://mollyrocket.com/849)

		//get point which belongs to the outline of the shape (Minkowski difference)
		Vector3<T> initialDirection = Vector3<T>(1.0, 0.0, 0.0);
		Point3<T> initialSupportPointA = convexObject1.getSupportPoint(initialDirection.template cast<float>(), includeMargin).template cast<T>();
		Point3<T> initialSupportPointB = convexObject2.getSupportPoint((-initialDirection).template cast<float>(), includeMargin).template cast<T>();
		Point3<T> initialPoint = initialSupportPointA - initialSupportPointB;

		Vector3<T> direction = initialPoint.vector(Point3<T>(0.0, 0.0, 0.0));
		Point3<T> closestPointOnSimplex = initialPoint;

		Simplex<T> simplex;
		simplex.addPoint(initialSupportPointA, initialSupportPointB);
		simplex.setBarycentric(0, 1.0);
		simplex.setClosestPointToOrigin(closestPointOnSimplex);

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
					return std::unique_ptr<GJKResultCollide<T>>(new GJKResultCollide<T>(simplex));
				}else
				{
					return std::unique_ptr<GJKResultNoCollide<T>>(new GJKResultNoCollide<T>(std::sqrt(closestPointSquareDistance), simplex));
				}
			}

			simplex.addPoint(supportPointA, supportPointB);
			updateSimplex(simplex);

			closestPointOnSimplex = simplex.getClosestPointToOrigin();
			direction = closestPointOnSimplex.vector(Point3<T>(0.0, 0.0, 0.0));

			minimumToleranceMultiplicator += percentageIncreaseOfMinimumTolerance;
		}

		#ifdef _DEBUG
			logMaximumIterationReach();
		#endif

		return std::unique_ptr<GJKResultInvalid<T>>(new GJKResultInvalid<T>());
	}

	/**
	 * Update the simplex and return closest point to origin on the simplex
	 * @return Closest point to origin on the simplex
	 */
	template<class T> void GJKAlgorithm<T>::updateSimplex(Simplex<T> &simplex) const
	{
		Point3<T> closestPoint(0.0, 0.0, 0.0);
		T barycentrics[4];

		if(simplex.getSize() == 2)
		{ //simplex is a line (1D)

			const Point3<T> &pointA = simplex.getPoint(0);
			const Point3<T> &pointB = simplex.getPoint(1); //pointB is the last point added to the simplex

			closestPoint = LineSegment3D<T>(pointA, pointB).closestPoint(Point3<T>(0.0, 0.0, 0.0), barycentrics);
			simplex.setBarycentric(0, barycentrics[0]);
			simplex.setBarycentric(1, barycentrics[1]);
		}else if(simplex.getSize() == 3)
		{ //simplex is a triangle (2D)

			const Point3<T> &pointA = simplex.getPoint(0);
			const Point3<T> &pointB = simplex.getPoint(1);
			const Point3<T> &pointC = simplex.getPoint(2); //pointC is the last point added to the simplex

			const Vector3<T> co = pointC.vector(Point3<T>(0.0, 0.0, 0.0));
			const Vector3<T> cb = pointC.vector(pointB);
			const Vector3<T> ca = pointC.vector(pointA);
			const Vector3<T> normalAbc = cb.crossProduct(ca);

			closestPoint = Triangle3D<T>(pointA, pointB, pointC).closestPoint(Point3<T>(0.0, 0.0, 0.0), barycentrics);
			simplex.setBarycentric(0, barycentrics[0]);
			simplex.setBarycentric(1, barycentrics[1]);
			simplex.setBarycentric(2, barycentrics[2]);

			if(barycentrics[1]==0.0)
			{ //remove pointB
				simplex.removePoint(1);
			}
			if(barycentrics[0]==0.0)
			{ //remove pointA
				simplex.removePoint(0);
			}

			if(normalAbc.dotProduct(co) <= 0.0)
			{ //voronoi region -ABC => ABC
				simplex.swapPoints(0, 1); //swap pointA and pointB
			}
		}else if (simplex.getSize() == 4)
		{ //simplex is a tetrahedron (3D)

			const Point3<T> &pointA = simplex.getPoint(0);
			const Point3<T> &pointB = simplex.getPoint(1);
			const Point3<T> &pointC = simplex.getPoint(2);
			const Point3<T> &pointD = simplex.getPoint(3); //pointD is the last point added to the simplex

			const short voronoiRegionMask = 14; //test all voronoi regions except the one which doesn't include the new point added (pointD)
			closestPoint = Tetrahedron<T>(pointA, pointB, pointC, pointD).closestPoint(Point3<T>(0.0, 0.0, 0.0), barycentrics, voronoiRegionMask);
			simplex.setBarycentric(0, barycentrics[0]);
			simplex.setBarycentric(1, barycentrics[1]);
			simplex.setBarycentric(2, barycentrics[2]);
			simplex.setBarycentric(3, barycentrics[3]);

			if(barycentrics[2]==0.0)
			{ //remove pointC
				simplex.removePoint(2);
			}
			if(barycentrics[1]==0.0)
			{ //remove pointB
				simplex.removePoint(1);
			}
			if(barycentrics[0]==0.0)
			{ //remove pointA
				simplex.removePoint(0);
			}
		}else
		{
			throw std::invalid_argument("Size of simplex unsupported: " + std::to_string(simplex.getSize()) + ".");
		}

		simplex.setClosestPointToOrigin(closestPoint);
	}

	#ifdef _DEBUG
		template<class T> void GJKAlgorithm<T>::logMaximumIterationReach() const
		{
			Logger::setLogger(new FileLogger());
			Logger::logger()<<Logger::prefix(Logger::LOG_WARNING);
			Logger::logger()<<"Maximum of iteration reached on GJK algorithm ("<<maxIteration<<")."<<"\n";
			Logger::logger()<<" - Relative termination tolerance: "<<relativeTerminationTolerance<<"\n";
			Logger::logger()<<" - Minimum termination tolerance: "<<minimumTerminationTolerance<<"\n";
			Logger::logger()<<" - Percentage increase of minimum tolerance: "<<percentageIncreaseOfMinimumTolerance<<"\n";
			Logger::setLogger(nullptr);
		}
	#endif

	//explicit template
	template class GJKAlgorithm<float>;
	template class GJKAlgorithm<double>;

}
