#include <limits>
#include <algorithm>
#include <stdexcept>

#include "collision/narrowphase/algorithm/utils/Simplex.h"

namespace urchin
{

	template<class T> inline Simplex<T>::Simplex()
	{
		simplexPoints.reserve(4);
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

		updateSimplex();
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
		for(auto it = simplexPoints.begin(); it!=simplexPoints.end(); ++it)
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

	/**
	 * Update the simplex: remove useless point, find closest point to origin and barycentrics.
	 */
	template<class T> void Simplex<T>::updateSimplex()
	{
		T barycentrics[4];

		if(getSize() == 1)
		{ //simplex is a point
			closestPointToOrigin = getPoint(0);

			setBarycentric(0, 1.0);
		}else if(getSize() == 2)
		{ //simplex is a line (1D)

			const Point3<T> &pointA = getPoint(0);
			const Point3<T> &pointB = getPoint(1); //pointB is the last point added to the simplex

			closestPointToOrigin = LineSegment3D<T>(pointA, pointB).closestPoint(Point3<T>(0.0, 0.0, 0.0), barycentrics);
			setBarycentric(0, barycentrics[0]);
			setBarycentric(1, barycentrics[1]);
		}else if(getSize() == 3)
		{ //simplex is a triangle (2D)

			const Point3<T> &pointA = getPoint(0);
			const Point3<T> &pointB = getPoint(1);
			const Point3<T> &pointC = getPoint(2); //pointC is the last point added to the simplex

			const Vector3<T> co = pointC.vector(Point3<T>(0.0, 0.0, 0.0));
			const Vector3<T> cb = pointC.vector(pointB);
			const Vector3<T> ca = pointC.vector(pointA);
			const Vector3<T> normalAbc = cb.crossProduct(ca);

			closestPointToOrigin = Triangle3D<T>(pointA, pointB, pointC).closestPoint(Point3<T>(0.0, 0.0, 0.0), barycentrics);
			setBarycentric(0, barycentrics[0]);
			setBarycentric(1, barycentrics[1]);
			setBarycentric(2, barycentrics[2]);

			if(barycentrics[1]==0.0)
			{ //remove pointB
				removePoint(1);
			}
			if(barycentrics[0]==0.0)
			{ //remove pointA
				removePoint(0);
			}

			if(normalAbc.dotProduct(co) <= 0.0)
			{ //voronoi region -ABC => ABC
				std::swap(simplexPoints[0], simplexPoints[1]); //swap pointA and pointB
			}
		}else if (getSize() == 4)
		{ //simplex is a tetrahedron (3D)

			const Point3<T> &pointA = getPoint(0);
			const Point3<T> &pointB = getPoint(1);
			const Point3<T> &pointC = getPoint(2);
			const Point3<T> &pointD = getPoint(3); //pointD is the last point added to the simplex

			const short voronoiRegionMask = 14; //test all voronoi regions except the one which doesn't include the new point added (pointD)
			closestPointToOrigin = Tetrahedron<T>(pointA, pointB, pointC, pointD).closestPoint(Point3<T>(0.0, 0.0, 0.0), barycentrics, voronoiRegionMask);
			setBarycentric(0, barycentrics[0]);
			setBarycentric(1, barycentrics[1]);
			setBarycentric(2, barycentrics[2]);
			setBarycentric(3, barycentrics[3]);

			if(barycentrics[2]==0.0)
			{ //remove pointC
				removePoint(2);
			}
			if(barycentrics[1]==0.0)
			{ //remove pointB
				removePoint(1);
			}
			if(barycentrics[0]==0.0)
			{ //remove pointA
				removePoint(0);
			}
		}else
		{
			throw std::invalid_argument("Size of simplex unsupported: " + std::to_string(getSize()) + ".");
		}
	}

	template<class T> inline void Simplex<T>::removePoint(unsigned int index)
	{
		simplexPoints.erase(simplexPoints.begin() + index);
	}

	template<class T> inline void Simplex<T>::setBarycentric(unsigned int index, T barycentric)
	{
		simplexPoints[index].barycentric = barycentric;
	}

	//explicit template
	template class Simplex<float>;
	template class Simplex<double>;

}
