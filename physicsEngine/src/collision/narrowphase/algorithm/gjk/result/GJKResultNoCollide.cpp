#include "GJKResultNoCollide.h"

namespace urchin
{

	/**
	 * @param separatingDistance Separating distance of two objects. In case of collision, the distance is zero.
	 */
	template<class T> GJKResultNoCollide<T>::GJKResultNoCollide(T separatingDistance, const Simplex<T> &simplex) :
		GJKResult<T>(),
		separatingDistance(separatingDistance),
		simplex(simplex)
	{
		simplex.computeClosestPoints(closestPointA, closestPointB);

		#ifdef _DEBUG //TODO add in log
			const float closestPointsLength = closestPointA.vector(closestPointB).length();
			const float subtractDistance = closestPointsLength - separatingDistance;
			assert((subtractDistance-0.01) <= 0.0 && (subtractDistance+0.01) >= 0.0);
		#endif
	}

	template<class T> bool GJKResultNoCollide<T>::isValidResult() const
	{
		return true;
	}

	template<class T> bool GJKResultNoCollide<T>::isCollide() const
	{
		return false;
	}

	/**
	 * @return Separating distance between convex hull (always positive)
	 */
	template<class T> T GJKResultNoCollide<T>::getSeparatingDistance() const
	{
		return separatingDistance;
	}

	template<class T> const Simplex<T> &GJKResultNoCollide<T>::getSimplex() const
	{
		return simplex;
	}

	/**
	 * @return Closest point on object A in case of non collision
	 */
	template<class T> const Point3<T> &GJKResultNoCollide<T>::getClosestPointA() const
	{
		return closestPointA;
	}

	/**
	 * @return Closest point on object B in case of non collision
	 */
	template<class T> const Point3<T> &GJKResultNoCollide<T>::getClosestPointB() const
	{
		return closestPointB;
	}

	//explicit template
	template class GJKResultNoCollide<float>;
	template class GJKResultNoCollide<double>;

}
