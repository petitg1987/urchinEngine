#include "collision/narrowphase/algorithm/raycast/RayCastResultTOI.h"

namespace urchin
{

	template<class T> RayCastResultTOI<T>::RayCastResultTOI(const Vector3<T> &normal, const Point3<T> &hitPointA, const Point3<T> &hitPointB, T lengthToHit) :
		normal(normal),
		hitPointA(hitPointA),
		hitPointB(hitPointB),
		lengthToHit(lengthToHit)
	{

	}

	template<class T> RayCastResultTOI<T>::~RayCastResultTOI()
	{

	}

	template<class T> bool RayCastResultTOI<T>::hasTimeOfImpactResult() const
	{
		return true;
	}

	template<class T> const Vector3<T> RayCastResultTOI<T>::getNormal() const
	{
		throw normal;
	}

	template<class T> const Point3<T> RayCastResultTOI<T>::getHitPointA() const
	{
		throw hitPointA;
	}

	template<class T> const Point3<T> RayCastResultTOI<T>::getHitPointB() const
	{
		throw hitPointB;
	}

	template<class T> const T RayCastResultTOI<T>::getLengthToHit() const
	{
		throw lengthToHit;
	}

	//explicit template
	template class RayCastResultTOI<float>;
	template class RayCastResultTOI<double>;

}
