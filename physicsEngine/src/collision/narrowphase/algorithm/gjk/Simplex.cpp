#include <limits>

#include "collision/narrowphase/algorithm/gjk/Simplex.h"

namespace urchin
{

	template<class T> inline Simplex<T>::Simplex()
	{
		simplexPoints.reserve(4);
	}

	template<class T> inline Simplex<T>::~Simplex()
	{

	}

	/**
	 * Add a point to the end of the simplex
	 * @return Point added (supportPointA - supportPointB)
	 */
	template<class T> void Simplex<T>::addPoint(const Point3<T> &supportPointA, const Point3<T> &supportPointB)
	{
		SupportMapping<T> supportMapping;
		supportMapping.supportPointA = supportPointA;
		supportMapping.supportPointB = supportPointB;
		supportMapping.point = supportPointA - supportPointB;
		supportMapping.barycentric = std::numeric_limits<T>::max();

		simplexPoints.push_back(supportMapping);
	}

	template<class T> inline void Simplex<T>::removePoint(unsigned int index)
	{
		simplexPoints.erase(simplexPoints.begin() + index);
	}

	template<class T> inline void Simplex<T>::swapPoints(unsigned int index1, unsigned int index2)
	{
		std::swap(simplexPoints[index1], simplexPoints[index2]);
	}

	template<class T> inline void Simplex<T>::setBarycentric(unsigned int index, T barycentric)
	{
		simplexPoints[index].barycentric = barycentric;
	}

	template<class T> inline void Simplex<T>::setClosestPointToOrigin(const Point3<T> &closestPointToOrigin)
	{
		this->closestPointToOrigin = closestPointToOrigin;
	}

	template<class T> inline unsigned int Simplex<T>::getSize() const
	{
		return simplexPoints.size();
	}

	template<class T> inline const Point3<T> &Simplex<T>::getPoint(unsigned int index) const
	{
		return simplexPoints[index].point;
	}

	template<class T> inline const Point3<T> &Simplex<T>::getSupportPointA(unsigned int index) const
	{
		return simplexPoints[index].supportPointA;
	}

	template<class T> inline const Point3<T> &Simplex<T>::getSupportPointB(unsigned int index) const
	{
		return simplexPoints[index].supportPointB;
	}

	template<class T> inline T Simplex<T>::getBarycentric(unsigned int index) const
	{
		return simplexPoints[index].barycentric;
	}

	/**
	 * @return Closest point to origin belonging to simplex
	 */
	template<class T> inline const Point3<T> &Simplex<T>::getClosestPointToOrigin() const
	{
		return closestPointToOrigin;
	}

	template<class T> bool Simplex<T>::isPointInSimplex(const Point3<T> &p) const
	{
		for(typename std::vector<SupportMapping<T>>::const_iterator it = simplexPoints.begin(); it!=simplexPoints.end(); ++it)
		{
			const Point3<T> &simplexPoint = it->point;
			if(simplexPoint.X==p.X && simplexPoint.Y==p.Y && simplexPoint.Z==p.Z)
			{
				return true;
			}
		}

		return false;
	}

	/**
	 * Computes closest points on objects A and B. If two objects collides, the behavior is undefined
	 * @param closestPointA [out] Computed closest point of object A
	 * @param closestPointB [out] Computed closest point of object B
	 */
	template<class T> void Simplex<T>::computeClosestPoints(Point3<T> &closestPointA, Point3<T> &closestPointB) const
	{
		if(simplexPoints.size() == 1)
		{ //simplex is a point

			closestPointA = simplexPoints[0].barycentric * simplexPoints[0].supportPointA;
			closestPointB = simplexPoints[0].barycentric * simplexPoints[0].supportPointB;
		}else if(simplexPoints.size() == 2)
		{ //simplex is a line (1D)

			closestPointA = simplexPoints[0].barycentric * simplexPoints[0].supportPointA + simplexPoints[1].barycentric * simplexPoints[1].supportPointA;
			closestPointB = simplexPoints[0].barycentric * simplexPoints[0].supportPointB + simplexPoints[1].barycentric * simplexPoints[1].supportPointB;
		}else if(simplexPoints.size() == 3)
		{ //simplex is a triangle (2D)

			closestPointA = simplexPoints[0].barycentric * simplexPoints[0].supportPointA + simplexPoints[1].barycentric * simplexPoints[1].supportPointA
					+ simplexPoints[2].barycentric * simplexPoints[2].supportPointA;
			closestPointB = simplexPoints[0].barycentric * simplexPoints[0].supportPointB + simplexPoints[1].barycentric * simplexPoints[1].supportPointB
					+ simplexPoints[2].barycentric * simplexPoints[2].supportPointB;
		}else if(simplexPoints.size() == 4)
		{ //simplex is a tetrahedron (3D)
			closestPointA = simplexPoints[0].barycentric * simplexPoints[0].supportPointA + simplexPoints[1].barycentric * simplexPoints[1].supportPointA
					+ simplexPoints[2].barycentric * simplexPoints[2].supportPointA + simplexPoints[3].barycentric * simplexPoints[3].supportPointA;
			closestPointB = simplexPoints[0].barycentric * simplexPoints[0].supportPointB + simplexPoints[1].barycentric * simplexPoints[1].supportPointB
					+ simplexPoints[2].barycentric * simplexPoints[2].supportPointB + simplexPoints[3].barycentric * simplexPoints[3].supportPointB;
		}else
		{
			throw std::invalid_argument("Size of simplex unsupported to compute closest points: " + std::to_string(simplexPoints.size()) + ".");
		}
	}

	//explicit template
	template class Simplex<float>;

}
