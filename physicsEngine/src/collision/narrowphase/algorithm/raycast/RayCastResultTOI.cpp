#include "collision/narrowphase/algorithm/raycast/RayCastResultTOI.h"

namespace urchin
{

	template<class T> RayCastResultTOI<T>::RayCastResultTOI(const Vector3<T> &normal, const Point3<T> &hitPointB, T timeToHit) :
		normal(normal),
		hitPointB(hitPointB),
		timeToHit(timeToHit)
	{

	}

	template<class T> RayCastResultTOI<T>::~RayCastResultTOI()
	{

	}

	template<class T> bool RayCastResultTOI<T>::hasTimeOfImpactResult() const
	{
		return true;
	}

	template<class T> const Vector3<T> &RayCastResultTOI<T>::getNormal() const
	{
		return normal;
	}

	template<class T> const Point3<T> &RayCastResultTOI<T>::getHitPointB() const
	{
		return hitPointB;
	}

	/**
	 * Return time to hit. A value of 0.0 means objects collide at initial situation (from transformation).
	 * A value of 1.0 means objects will collide at final situation (to transformation).
	 */
	template<class T> T RayCastResultTOI<T>::getTimeToHit() const
	{
		return timeToHit;
	}

	//explicit template
	template class RayCastResultTOI<float>;
	template class RayCastResultTOI<double>;

}
