#include <cmath>

#include "Cone.h"

namespace urchin
{

	template<class T> Cone<T>::Cone() :
			coneShape(ConeShape<T>(0.0, 0.0, ConeShape<T>::CONE_X_POSITIVE)),
			centerPosition(Point3<T>(0.0, 0.0, 0.0))
	{
		axis[0] = Vector3<T>(0.0, 0.0, 0.0);
		axis[1] = Vector3<T>(0.0, 0.0, 0.0);
		axis[2] = Vector3<T>(0.0, 0.0, 0.0);

		baseSideCosAngle = 0.0;
	}

	template<class T> Cone<T>::Cone(T radius, T height, typename ConeShape<T>::ConeOrientation coneOrientation,
			const Point3<T> &centerPosition, const Quaternion<T> &orientation) :
		coneShape(ConeShape<T>(radius, height, coneOrientation)),
		centerPosition(centerPosition),
		orientation(orientation)
	{
		axis[0] = orientation.rotatePoint(Point3<T>(1.0, 0.0, 0.0)).toVector();
		axis[1] = orientation.rotatePoint(Point3<T>(0.0, 1.0, 0.0)).toVector();
		axis[2] = orientation.rotatePoint(Point3<T>(0.0, 0.0, 1.0)).toVector();

		baseSideCosAngle = radius / sqrt(radius * radius + height * height);
	}

	template<class T> Cone<T>::~Cone()
	{

	}

	template<class T> T Cone<T>::getRadius() const
	{
		return coneShape.getRadius();
	}

	template<class T> T Cone<T>::getHeight() const
	{
		return coneShape.getHeight();
	}

	template<class T> typename ConeShape<T>::ConeOrientation Cone<T>::getConeOrientation() const
	{
		return coneShape.getConeOrientation();
	}

	/**
	 * @return Point at middle height of the cone
	 */
	template<class T> const Point3<T> &Cone<T>::getCenterPosition() const
	{
		return centerPosition;
	}

	template<class T> const Quaternion<T> &Cone<T>::getOrientation() const
	{
		return orientation;
	}

	/**
	 * @return Cone normalized axis for given index
	 */
	template<class T> const Vector3<T> &Cone<T>::getAxis(unsigned int index) const
	{
		return axis[index];
	}

	template<class T> Point3<T> Cone<T>::getSupportPoint(const Vector3<T> &direction) const
	{
		T halfHeight = (T)(getHeight() / (T)2.0);
		Vector3<T> centralAxis = axis[getConeOrientation()/2] * ((getConeOrientation()%2==0) ? (T)1.0 : (T)-1.0); //axis from base to top
		Vector3<T> normalizedDirection;
		if(direction.X==0.0 && direction.Y==0.0 && direction.Z==0.0)
		{
			normalizedDirection = Vector3<T>(1.0, 0.0, 0.0);
		}else
		{
			normalizedDirection = direction.normalize();
		}

		T centralDirectionCosAngle = centralAxis.dotProduct(normalizedDirection);
		if(centralDirectionCosAngle > baseSideCosAngle)
		{ //support point = top of cone
			return centerPosition.translate(centralAxis * halfHeight);
		}

		Vector3<T> projectedDirectionOnCircle = normalizedDirection - (normalizedDirection.dotProduct(centralAxis) * centralAxis);
		if(projectedDirectionOnCircle.squareLength() > 0.0)
		{
			projectedDirectionOnCircle = projectedDirectionOnCircle.normalize();
		}

		Point3<T> bottomPosition = centerPosition.translate(centralAxis * (-halfHeight));
		return bottomPosition.translate(projectedDirectionOnCircle * getRadius());
	}

	//explicit template
	template class Cone<float>;
	template class Cone<double>;

}
