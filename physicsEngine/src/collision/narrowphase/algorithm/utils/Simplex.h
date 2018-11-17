#ifndef URCHINENGINE_SIMPLEX_H
#define URCHINENGINE_SIMPLEX_H

#include <vector>
#include "UrchinCommon.h"

namespace urchin
{

	template<class T> struct SupportMapping
	{
		Point3<T> supportPointA;
		Point3<T> supportPointB;
		Point3<T> point; //supportPointA - supportPointB
		T barycentric; //barycentric/weight of point 'point'. Value undefined when simplex contain the origin.
	};

	template<class T> class Simplex
	{
		public:
			Simplex();

			void addPoint(const Point3<T> &, const Point3<T> &);

			unsigned int getSize() const;
			const Point3<T> &getPoint(unsigned int) const;
			const Point3<T> &getSupportPointA(unsigned int) const;
			const Point3<T> &getSupportPointB(unsigned int) const;
			T getBarycentric(unsigned int) const;
			const Point3<T> &getClosestPointToOrigin() const;
			bool isPointInSimplex(const Point3<T> &) const;

			void computeClosestPoints(Point3<T> &, Point3<T> &) const;

		private:
			void updateSimplex();
			void removePoint(unsigned int);
			void setBarycentric(unsigned int, T);

			SupportMapping<T> simplexPoints[4];
			unsigned int simplexPointsSize;
			Point3<T> closestPointToOrigin;
	};

}

#endif
