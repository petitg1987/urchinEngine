#include "Tetrahedron.h"

namespace urchin
{

	template<class T> Tetrahedron<T>::Tetrahedron(const Point3<T> &a, const Point3<T> &b, const Point3<T> &c, const Point3<T> &d):
		a(a), b(b), c(c), d(d)
	{

	}

	template<class T> Point3<T> Tetrahedron<T>::getSupportPoint(const Vector3<T> &direction) const
	{
		const T pointADotDirection = a.toVector().dotProduct(direction);
		const T pointBDotDirection = b.toVector().dotProduct(direction);
		const T pointCDotDirection = c.toVector().dotProduct(direction);
		const T pointDDotDirection = d.toVector().dotProduct(direction);

		T maxPointDotDirection = pointADotDirection;
		Point3<T> supportPoint = a;
		if(pointBDotDirection > maxPointDotDirection)
		{
			maxPointDotDirection = pointBDotDirection;
			supportPoint = b;
		}

		if(pointCDotDirection > maxPointDotDirection)
		{
			maxPointDotDirection = pointCDotDirection;
			supportPoint = c;
		}

		if(pointDDotDirection > maxPointDotDirection)
		{
			maxPointDotDirection = pointDDotDirection;
			supportPoint = d;
		}

		return supportPoint;
	}

	/**
	 * @param barycentrics [out] Returns barycentric coordinates for closestPoint only when closestPoint!=p
	 * @param voronoiRegionMask Mask allowing to include/exclude some voronoi regions to test. The bits from right to left represent
	 * respectively the following voronoi regions: ABC, ACD, ADB, BDC.
	 * @return Point on tetrahedron closest to point p
	 */
	template<class T> Point3<T> Tetrahedron<T>::closestPoint(const Point3<T> &p, T barycentrics[4], short voronoiRegionMask) const
	{
		T triangleBarycentrics[3];
		T bestSquareDist = std::numeric_limits<T>::max();

		//start by assuming point p is inside the tetrahedron
		Point3<T> closestPoint = p;
		barycentrics[0] = std::numeric_limits<T>::max();
		barycentrics[1] = std::numeric_limits<T>::max();
		barycentrics[2] = std::numeric_limits<T>::max();
		barycentrics[3] = std::numeric_limits<T>::max();

		//if point outside face ABC, then compute closest point on ABC
		if((voronoiRegionMask & 1) && pointOutsidePlane(p, a, b, c, d))
		{
			Point3<T> q = Triangle3D<T>(a, b, c).closestPoint(p, triangleBarycentrics);

			Vector3<T> pq = p.vector(q);
			T squareDist = pq.dotProduct(pq);
			if(squareDist < bestSquareDist)
			{
				bestSquareDist = squareDist;

				closestPoint = q;
				barycentrics[0] = triangleBarycentrics[0];
				barycentrics[1] = triangleBarycentrics[1];
				barycentrics[2] = triangleBarycentrics[2];
				barycentrics[3] = 0.0;
			}
		}

		//if point outside face ACD, then compute closest point on ACD
		if((voronoiRegionMask & 2) && pointOutsidePlane(p, a, c, d, b))
		{
			Point3<T> q = Triangle3D<T>(a, c, d).closestPoint(p, triangleBarycentrics);

			Vector3<T> pq = p.vector(q);
			T squareDist = pq.dotProduct(pq);
			if(squareDist < bestSquareDist)
			{
				bestSquareDist = squareDist;

				closestPoint = q;
				barycentrics[0] = triangleBarycentrics[0];
				barycentrics[1] = 0.0;
				barycentrics[2] = triangleBarycentrics[1];
				barycentrics[3] = triangleBarycentrics[2];
			}
		}

		//if point outside face ADB, then compute closest point on ADB
		if((voronoiRegionMask & 4) && pointOutsidePlane(p, a, d, b, c))
		{
			Point3<T> q = Triangle3D<T>(a, d, b).closestPoint(p, triangleBarycentrics);

			Vector3<T> pq = p.vector(q);
			T squareDist = pq.dotProduct(pq);
			if(squareDist < bestSquareDist)
			{
				bestSquareDist = squareDist;

				closestPoint = q;
				barycentrics[0] = triangleBarycentrics[0];
				barycentrics[1] = triangleBarycentrics[2];
				barycentrics[2] = 0.0;
				barycentrics[3] = triangleBarycentrics[1];
			}
		}

		//if point outside face BDC, then compute closest point on BDC
		if((voronoiRegionMask & 8) && pointOutsidePlane(p, b, d, c, a))
		{
			Point3<T> q = Triangle3D<T>(b, d, c).closestPoint(p, triangleBarycentrics);

			Vector3<T> pq = p.vector(q);
			T squareDist = pq.dotProduct(pq);
			if(squareDist < bestSquareDist)
			{
				bestSquareDist = squareDist;

				closestPoint = q;
				barycentrics[0] = 0.0;
				barycentrics[1] = triangleBarycentrics[0];
				barycentrics[2] = triangleBarycentrics[2];
				barycentrics[3] = triangleBarycentrics[1];
			}
		}

		return closestPoint;
	}

	template<class T> bool Tetrahedron<T>::collideWithPoint(const Point3<T> &p) const
	{
		if(pointOutsidePlane(p, a, b, c, d))
		{
			return false;
		}

		if(pointOutsidePlane(p, a, c, d, b))
		{
			return false;
		}

		if(pointOutsidePlane(p, a, d, b, c))
		{
			return false;
		}

		if(pointOutsidePlane(p, b, d, c, a))
		{
			return false;
		}

		return true;
	}

	/**
	 * @return True if point p is outside the plane. Direction of plane normal is determinate by d.
	 */
	template<class T> bool Tetrahedron<T>::pointOutsidePlane(const Point3<T> &p,
		const Point3<T> &planePointA, const Point3<T> &planePointB, const Point3<T> &planePointC, const Point3<T> &d) const
	{
		Vector3<T> ap = planePointA.vector(p);
		Vector3<T> ad = planePointA.vector(d);
		Vector3<T> ab = planePointA.vector(planePointB);
		Vector3<T> ac = planePointA.vector(planePointC);

		T signp = ap.dotProduct(ab.crossProduct(ac));
		T signd = ad.dotProduct(ab.crossProduct(ac));

		return (signp * signd) < (T)0.0;
	}

	//explicit template
	template class Tetrahedron<float>;

}
