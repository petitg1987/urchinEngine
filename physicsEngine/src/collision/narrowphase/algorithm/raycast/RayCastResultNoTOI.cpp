#include <stdexcept>

#include "collision/narrowphase/algorithm/raycast/RayCastResultNoTOI.h"

namespace urchin
{

	template<class T> RayCastResultNoTOI<T>::RayCastResultNoTOI()
	{

	}

	template<class T> RayCastResultNoTOI<T>::~RayCastResultNoTOI()
	{

	}

	template<class T> bool RayCastResultNoTOI<T>::hasTimeOfImpactResult() const
	{
		return false;
	}

	template<class T> const Vector3<T> &RayCastResultNoTOI<T>::getNormal() const
	{
		throw std::domain_error("No time of impact: cannot call this method.");
	}

	template<class T> const Point3<T> &RayCastResultNoTOI<T>::getHitPointB() const
	{
		throw std::domain_error("No time of impact: cannot call this method.");
	}

	template<class T> T RayCastResultNoTOI<T>::getTimeToHit() const
	{
		throw std::domain_error("No time of impact: cannot call this method.");
	}

	//explicit template
	template class RayCastResultNoTOI<float>;
	template class RayCastResultNoTOI<double>;

}
