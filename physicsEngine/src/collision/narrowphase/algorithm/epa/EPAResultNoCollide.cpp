#include "collision/narrowphase/algorithm/epa/EPAResultNoCollide.h"

namespace urchin
{

	template<class T> EPAResultNoCollide<T>::EPAResultNoCollide() :
		EPAResult<T>()
	{

	}

	template<class T> EPAResultNoCollide<T>::~EPAResultNoCollide()
	{

	}

	template<class T> bool EPAResultNoCollide<T>::isValidResult() const
	{
		return true;
	}

	template<class T> bool EPAResultNoCollide<T>::isCollide() const
	{
		return false;
	}

	template<class T> const Point3<T> &EPAResultNoCollide<T>::getContactPointA() const
	{
		throw std::domain_error("EPA algorithm doesn't provide contact points when bodies not collide.");
	}

	template<class T> const Point3<T> &EPAResultNoCollide<T>::getContactPointB() const
	{
		throw std::domain_error("EPA algorithm doesn't provide contact points when bodies not collide.");
	}

	template<class T> const Vector3<T> &EPAResultNoCollide<T>::getNormal() const
	{
		throw std::domain_error("EPA algorithm doesn't provide normal when bodies not collide.");
	}

	template<class T> T EPAResultNoCollide<T>::getPenetrationDepth() const
	{
		throw std::domain_error("EPA algorithm doesn't provide penetration depth when bodies not collide.");
	}

	//explicit template
	template class EPAResultNoCollide<float>;
	template class EPAResultNoCollide<double>;

}
