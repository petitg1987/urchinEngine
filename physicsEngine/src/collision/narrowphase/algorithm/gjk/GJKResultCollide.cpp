#include "collision/narrowphase/algorithm/gjk/GJKResultCollide.h"

namespace urchin
{

	template<class T> GJKResultCollide<T>::GJKResultCollide(const Simplex<T> &simplex) :
		GJKResult<T>(),
		simplex(simplex)
	{

	}

	template<class T> bool GJKResultCollide<T>::isValidResult() const
	{
		return true;
	}

	template<class T> bool GJKResultCollide<T>::isCollide() const
	{
		return true;
	}

	/**
	 * @return Separating distance between convex hull (always positive)
	 */
	template<class T> T GJKResultCollide<T>::getSeparatingDistance() const
	{
		throw std::domain_error("GJK algorithm doesn't provide separating distance when bodies collide.");
	}

	template<class T> const Simplex<T> &GJKResultCollide<T>::getSimplex() const
	{
		return simplex;
	}

	/**
	 * @return Closest point on object A in case of non collision
	 */
	template<class T> const Point3<T> &GJKResultCollide<T>::getClosestPointA() const
	{
		throw std::domain_error("GJK algorithm doesn't provide closest points when bodies collide.");
	}

	/**
	 * @return Closest point on object B in case of non collision
	 */
	template<class T> const Point3<T> &GJKResultCollide<T>::getClosestPointB() const
	{
		throw std::domain_error("GJK algorithm doesn't provide closest points when bodies collide.");
	}

	//explicit template
	template class GJKResultCollide<float>;
	template class GJKResultCollide<double>;

}
