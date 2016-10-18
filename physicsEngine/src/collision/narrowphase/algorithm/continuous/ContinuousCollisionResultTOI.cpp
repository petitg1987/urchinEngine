#include "collision/narrowphase/algorithm/continuous/ContinuousCollisionResultTOI.h"

namespace urchin
{

	template<class T> ContinuousCollisionResultTOI<T>::ContinuousCollisionResultTOI(const Vector3<T> &normal, const Point3<T> &hitPointB, T timeToHit) :
		normal(normal),
		hitPointB(hitPointB),
		timeToHit(timeToHit)
	{

	}

	template<class T> ContinuousCollisionResultTOI<T>::~ContinuousCollisionResultTOI()
	{

	}

	template<class T> bool ContinuousCollisionResultTOI<T>::hasTimeOfImpactResult() const
	{
		return true;
	}

	template<class T> const Vector3<T> &ContinuousCollisionResultTOI<T>::getNormal() const
	{
		return normal;
	}

	template<class T> const Point3<T> &ContinuousCollisionResultTOI<T>::getHitPointB() const
	{
		return hitPointB;
	}

	/**
	 * Return time to hit. A value of 0.0 means objects collide at initial situation (from transformation).
	 * A value of 1.0 means objects will collide at final situation (to transformation).
	 */
	template<class T> T ContinuousCollisionResultTOI<T>::getTimeToHit() const
	{
		return timeToHit;
	}

	//explicit template
	template class ContinuousCollisionResultTOI<float>;
	template class ContinuousCollisionResultTOI<double>;

}
