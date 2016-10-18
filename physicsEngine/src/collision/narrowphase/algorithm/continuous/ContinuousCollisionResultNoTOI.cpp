#include <stdexcept>

#include "collision/narrowphase/algorithm/continuous/ContinuousCollisionResultNoTOI.h"

namespace urchin
{

	template<class T> ContinuousCollisionResultNoTOI<T>::ContinuousCollisionResultNoTOI()
	{

	}

	template<class T> ContinuousCollisionResultNoTOI<T>::~ContinuousCollisionResultNoTOI()
	{

	}

	template<class T> bool ContinuousCollisionResultNoTOI<T>::hasTimeOfImpactResult() const
	{
		return false;
	}

	template<class T> const Vector3<T> &ContinuousCollisionResultNoTOI<T>::getNormal() const
	{
		throw std::domain_error("No time of impact: cannot call this method.");
	}

	template<class T> const Point3<T> &ContinuousCollisionResultNoTOI<T>::getHitPointB() const
	{
		throw std::domain_error("No time of impact: cannot call this method.");
	}

	template<class T> T ContinuousCollisionResultNoTOI<T>::getTimeToHit() const
	{
		throw std::domain_error("No time of impact: cannot call this method.");
	}

	//explicit template
	template class ContinuousCollisionResultNoTOI<float>;
	template class ContinuousCollisionResultNoTOI<double>;

}
