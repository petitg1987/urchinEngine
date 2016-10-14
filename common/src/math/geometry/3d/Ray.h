#ifndef ENGINE_RAY_H
#define ENGINE_RAY_H

#include "math/algebra/point/Point3.h"
#include "math/algebra/vector/Vector3.h"

namespace urchin
{

	template<class T> class Ray
	{
		public:
			Ray(const Point3<T> &, const Vector3<T> &, T);
			Ray(const Point3<T> &, const Point3<T> &);

			const Point3<T> &getOrigin() const;
			Point3<T> computeTo() const;

			const Vector3<T> &getDirection() const;
			T getLength() const;

			const Vector3<T> &getInverseDirection() const;
			unsigned int getDirectionSign(unsigned int) const;

		private:
			Point3<T> origin;
			Vector3<T> direction; //normalized
			T length;

			//additional data for performance (ray tracing, collision...)
			Vector3<T> inverseDirection;
			unsigned int directionSigns[3];
	};

}

#endif
